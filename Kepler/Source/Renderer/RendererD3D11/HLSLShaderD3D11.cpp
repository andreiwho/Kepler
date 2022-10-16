#include "HLSLShaderD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Renderer/RenderGlobals.h"

#include "D3D11Common.h"
#include "Async/Async.h"
#include "../Elements/ShaderReflection.h"
#include "../World/WorldRenderer.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogShaderReflection, All);

	//////////////////////////////////////////////////////////////////////////
	THLSLShaderD3D11::THLSLShaderD3D11(const TString& Name, const Array<ShaderModule>& Modules)
		: HLSLShader(Name, Modules)
	{
		InitHandle();
		InitShaders(Modules);
		InitReflection(Modules);
	}

	//////////////////////////////////////////////////////////////////////////
	void THLSLShaderD3D11::InitHandle()
	{
		m_Handle = MakeRef(New<TShaderHandleD3D11>());
		m_Handle->StageMask = m_ShaderStageMask;	// We have this being setup inside the TShader
	}

	//////////////////////////////////////////////////////////////////////////
	void THLSLShaderD3D11::InitShaders(const Array<ShaderModule>& Modules)
	{
		// CHECK(IsRenderThread());
		auto Device = TRenderDeviceD3D11::Get();
		if (Modules.GetLength() > 0)
		{
			CHECK(Device);
		}

		for (const auto& Module : Modules)
		{
			if (Module.ByteCode && VALIDATED(Module.ByteCode->GetSize() > 0))
			{
				if (Module.StageFlags & EShaderStageFlags::Vertex)
				{
					HRCHECK(Device->GetDevice()->CreateVertexShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->VertexShader
					));
				}

				if (Module.StageFlags & EShaderStageFlags::Pixel)
				{
					HRCHECK(Device->GetDevice()->CreatePixelShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->PixelShader
					));
				}

				if (Module.StageFlags & EShaderStageFlags::Compute)
				{
					HRCHECK(Device->GetDevice()->CreateComputeShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->ComputeShader
					));
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void THLSLShaderD3D11::InitReflection(const Array<ShaderModule>& Modules)
	{
		m_ReflectionData = MakeRef(New<TShaderModuleReflection>());
		for (const auto& Module : Modules)
		{
			CComPtr<ID3D11ShaderReflection> pReflection;
			HRCHECK(D3DReflect(Module.ByteCode->GetData(), Module.ByteCode->GetSize(), IID_PPV_ARGS(&pReflection)));

			m_ReflectionData->ParamMapping = ReflectParams(pReflection, Module.StageFlags, m_ReflectionData->ParamMapping ? m_ReflectionData->ParamMapping : nullptr);
			if (Module.StageFlags & EShaderStageFlags::Vertex)
			{
				m_ReflectionData->VertexLayout = ReflectVertexLayout(pReflection, Module);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		EShaderInputType DeduceInputParameterType(const D3D11_SIGNATURE_PARAMETER_DESC& Desc)
		{
			switch (Desc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
			{
				switch (Desc.Mask)
				{
				case D3D_COMPONENT_MASK_X:
					return EShaderInputType::Float;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y:
					return EShaderInputType::Float2;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z:
					return EShaderInputType::Float3;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z | D3D_COMPONENT_MASK_W:
					return EShaderInputType::Float4;
				}
			}
			break;
			case D3D_REGISTER_COMPONENT_SINT32:
				switch (Desc.Mask)
				{
				case D3D_COMPONENT_MASK_X:
					return EShaderInputType::Int;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y:
					return EShaderInputType::Int2;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z:
					return EShaderInputType::Int3;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z | D3D_COMPONENT_MASK_W:
					return EShaderInputType::Int4;
				}
				break;
			case D3D_REGISTER_COMPONENT_UINT32:
				switch (Desc.Mask)
				{
				case D3D_COMPONENT_MASK_X:
					return EShaderInputType::UInt;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y:
					return EShaderInputType::UInt2;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z:
					return EShaderInputType::UInt3;
				case D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z | D3D_COMPONENT_MASK_W:
					return EShaderInputType::UInt4;
				}
				break;
			case D3D_REGISTER_COMPONENT_UNKNOWN:
				return EShaderInputType::Custom;
			default:
				CRASH();
			}
			CRASH();
		}

		EShaderInputType ReflectVariableType(const D3D11_SHADER_TYPE_DESC& Desc)
		{
			switch (Desc.Class)
			{
			case D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_MATRIX_COLUMNS:
			{
				if (Desc.Columns == 3)
				{
					if (Desc.Rows == 3)
					{
						return EShaderInputType::Matrix3x3;
					}
					else if (Desc.Rows == 4)
					{
						return EShaderInputType::Matrix4x3;
					}
				}
				else if (Desc.Columns == 4)
				{
					if (Desc.Rows == 3)
					{
						return EShaderInputType::Matrix3x4;
					}
					if (Desc.Rows == 4)
					{
						return EShaderInputType::Matrix4x4;
					}
				}
			}
			break;
			case D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_VECTOR:
			{
				// TODO: Do we nead other vector types
				return EShaderInputType::Float4;
			}
			case D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_SCALAR:
			{
				switch (Desc.Type)
				{
				case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_FLOAT:
					return EShaderInputType::Float;
				case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_INT:
					return EShaderInputType::Int;
				case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_UINT:
					return EShaderInputType::UInt;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
			return EShaderInputType::Custom;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	VertexLayout THLSLShaderD3D11::ReflectVertexLayout(CComPtr<ID3D11ShaderReflection> pReflection, const ShaderModule& VertexShaderModule)
	{
		CHECK(VertexShaderModule.StageFlags & EShaderStageFlags::Vertex);
		CHECK(pReflection);
		VertexLayout OutLayout;

		D3D11_SHADER_DESC ShaderDesc{};
		HRCHECK(pReflection->GetDesc(&ShaderDesc));

		const UINT InputParamCount = ShaderDesc.InputParameters;

		usize ElemOffset = 0;
		for (UINT idx = 0; idx < InputParamCount; ++idx)
		{
			D3D11_SIGNATURE_PARAMETER_DESC InputParamDesc;
			HRCHECK(pReflection->GetInputParameterDesc(idx, &InputParamDesc));

			VertexAttribute Attribute{};
			Attribute.AttributeName = InputParamDesc.SemanticName;
			Attribute.AttributeId = InputParamDesc.SemanticIndex;
			Attribute.InputType = DeduceInputParameterType(InputParamDesc);
			Attribute.Offset = (UINT)ElemOffset;

			ElemOffset += Attribute.InputType.GetValueSize();

			OutLayout.AddAttribute(Attribute);
		}

		return OutLayout;
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<PipelineParamMapping> THLSLShaderD3D11::ReflectParams(CComPtr<ID3D11ShaderReflection> pReflection, EShaderStageFlags StageFlags, RefPtr<PipelineParamMapping> ToMerge)
	{
		D3D11_SHADER_DESC Desc;
		HRCHECK(pReflection->GetDesc(&Desc));
		
		RefPtr<PipelineParamMapping> ParamMappings = ToMerge ? ToMerge : PipelineParamMapping::New();
		for (UINT idx = 0; idx < Desc.ConstantBuffers; ++idx)
		{
			// Reflect constant buffers
			ID3D11ShaderReflectionConstantBuffer* pBuffer = pReflection->GetConstantBufferByIndex(idx);
			CHECK(pBuffer);
			D3D11_SHADER_BUFFER_DESC BufferDesc;
			HRCHECK(pBuffer->GetDesc(&BufferDesc));
			if (BufferDesc.Type == D3D_CBUFFER_TYPE::D3D_CT_CBUFFER)
			{
				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				HRCHECK(pReflection->GetResourceBindingDescByName(BufferDesc.Name, &bindDesc));
				if (bindDesc.BindPoint < TWorldRenderer::RS_User)
				{
					continue;
				}

				for (UINT VarIndex = 0; VarIndex < BufferDesc.Variables; ++VarIndex)
				{
					ID3D11ShaderReflectionVariable* pVar = pBuffer->GetVariableByIndex(VarIndex);
					CHECK(pVar);
					D3D11_SHADER_VARIABLE_DESC VarDesc;
					HRCHECK(pVar->GetDesc(&VarDesc));

					ID3D11ShaderReflectionType* pType = pVar->GetType();
					CHECK(pType);
					D3D11_SHADER_TYPE_DESC TypeDesc;
					HRCHECK(pType->GetDesc(&TypeDesc));
					ParamMappings->AddParam(VarDesc.Name, VarDesc.StartOffset, VarDesc.Size, StageFlags, ReflectVariableType(TypeDesc));
				}
			}
		}

		for (UINT idx = 0; idx < Desc.BoundResources; ++idx)
		{
			D3D11_SHADER_INPUT_BIND_DESC BindDesc;
			HRCHECK(pReflection->GetResourceBindingDesc(idx, &BindDesc));

			if (BindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D10_SIT_SAMPLER)
			{
				ParamMappings->AddTextureSampler(BindDesc.Name, StageFlags, BindDesc.BindPoint);
			}
		}

		return ParamMappings;
	}

	//////////////////////////////////////////////////////////////////////////
	TShaderHandleD3D11::~TShaderHandleD3D11()
	{
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK_NOTHROW(Device);
		if (VertexShader)
		{
			Device->RegisterPendingDeleteResource(VertexShader);
		}
		if (PixelShader)
		{
			Device->RegisterPendingDeleteResource(PixelShader);
		}
		if (ComputeShader)
		{
			Device->RegisterPendingDeleteResource(ComputeShader);
		}
	}

	//////////////////////////////////////////////////////////////////////////
}

