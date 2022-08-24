#include "HLSLShaderD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Renderer/RenderGlobals.h"

#include "D3D11Common.h"
#include "Async/Async.h"
#include "../Elements/ShaderReflection.h"

namespace Kepler
{

	THLSLShaderD3D11::THLSLShaderD3D11(const TString& Name, const TDynArray<TShaderModule>& Modules)
		: THLSLShader(Name, Modules)
	{
		InitHandle();
		InitShaders(Modules);
		InitReflection(Modules);
	}

	void THLSLShaderD3D11::InitHandle()
	{
		Handle = MakeRef(New<TShaderHandleD3D11>());
		Handle->StageMask = ShaderStageMask;	// We have this being setup inside the TShader
	}

	void THLSLShaderD3D11::InitShaders(const TDynArray<TShaderModule>& Modules)
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

	void THLSLShaderD3D11::InitReflection(const TDynArray<TShaderModule>& Modules)
	{
		ReflectionData = MakeRef(New<TShaderModuleReflection>());
		for (const auto& Module : Modules)
		{
			CComPtr<ID3D11ShaderReflection> pReflection;
			D3DReflect(Module.ByteCode->GetData(), Module.ByteCode->GetSize(), IID_PPV_ARGS(&pReflection));

			if (Module.StageFlags & EShaderStageFlags::Vertex)
			{
				ReflectionData->VertexLayout = ReflectVertexLayout(pReflection, Module);
			}
		}
	}

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
	}

	TVertexLayout THLSLShaderD3D11::ReflectVertexLayout(CComPtr<ID3D11ShaderReflection> pReflection, const TShaderModule& VertexShaderModule)
	{
		CHECK(VertexShaderModule.StageFlags & EShaderStageFlags::Vertex);
		CHECK(pReflection);
		TVertexLayout OutLayout;

		D3D11_SHADER_DESC ShaderDesc{};
		HRCHECK(pReflection->GetDesc(&ShaderDesc));

		const UINT InputParamCount = ShaderDesc.InputParameters;

		usize ElemOffset = 0;
		for (UINT Index = 0; Index < InputParamCount; ++Index)
		{
			D3D11_SIGNATURE_PARAMETER_DESC InputParamDesc;
			HRCHECK(pReflection->GetInputParameterDesc(Index, &InputParamDesc));

			TVertexAttribute Attribute{};
			Attribute.AttributeName = InputParamDesc.SemanticName;
			Attribute.AttributeId = InputParamDesc.SemanticIndex;
			Attribute.InputType = DeduceInputParameterType(InputParamDesc);
			Attribute.Offset = (UINT)ElemOffset;

			ElemOffset += Attribute.InputType.GetValueSize();

			OutLayout.AddAttribute(Attribute);
		}

		return OutLayout;
	}

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

}

