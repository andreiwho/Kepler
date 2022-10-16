#include "GraphicsPipelineHandleD3D11.h"
#include "Renderer/RendererD3D11/RenderDeviceD3D11.h"
#include "../RenderGlobals.h"
#include "../RenderTypes.h"
#include <type_traits>

namespace ke
{

	TGraphicsPipelineHandleD3D11::TGraphicsPipelineHandleD3D11(RefPtr<IShader> Shader, const TGraphicsPipelineConfiguration& Config)
	{
		SetupRasterizer(Config);
		SetupDepthStencil(Config);
		SetupInputLayout(Shader, Config);
	}

	TGraphicsPipelineHandleD3D11::~TGraphicsPipelineHandleD3D11()
	{
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (!Device)
		{
			return;
		}

		if (RasterState)
		{
			Device->RegisterPendingDeleteResource(RasterState);
		}

		if (DepthStencil)
		{
			Device->RegisterPendingDeleteResource(DepthStencil);
		}

		if (InputLayout)
		{
			Device->RegisterPendingDeleteResource(InputLayout);
		}
	}

	void TGraphicsPipelineHandleD3D11::SetupRasterizer(const TGraphicsPipelineConfiguration& Config)
	{
		CHECK(IsRenderThread());
		CD3D11_RASTERIZER_DESC Desc(D3D11_DEFAULT);
		Desc.ScissorEnable = Config.Rasterizer.bEnableScissor;

		if (Config.Rasterizer.bRasterDisabled)
		{
			return;
		}
		Desc.FillMode = std::invoke(
			[&Config]
			{
				switch (Config.Rasterizer.FillMode)
				{
				case EPrimitiveFillMode::Solid:
					return D3D11_FILL_SOLID;
				case EPrimitiveFillMode::Wireframe:
					return D3D11_FILL_WIREFRAME;
				}
				CRASH();
			});

		Desc.CullMode = std::invoke([&Config]
			{
				switch (Config.Rasterizer.CullMode)
				{
				case EPrimitiveCullMode::Back:
					return D3D11_CULL_BACK;
				case EPrimitiveCullMode::Front:
					return D3D11_CULL_FRONT;
				case EPrimitiveCullMode::None:
					return D3D11_CULL_NONE;
				}
				CRASH();
			});

		auto Device = CHECKED(TRenderDeviceD3D11::Get())->GetDevice();
		CHECK(Device);
		HRCHECK(Device->CreateRasterizerState(&Desc, &RasterState));
	}

	void TGraphicsPipelineHandleD3D11::SetupDepthStencil(const TGraphicsPipelineConfiguration& Config)
	{
		CHECK(IsRenderThread());
		CD3D11_DEPTH_STENCIL_DESC Desc(D3D11_DEFAULT);
		Desc.DepthEnable = Config.DepthStencil.bDepthEnable;
		Desc.DepthWriteMask = std::invoke(
			[&Config]
			{
				if (Config.DepthStencil.DepthAccess & EDepthBufferAccess::Write)
				{
					return D3D11_DEPTH_WRITE_MASK_ALL;
				}
				else
				{
					return D3D11_DEPTH_WRITE_MASK_ZERO;
				}
			});

		Desc.DepthFunc = std::invoke(
			[&Config]
			{
				switch (Config.DepthStencil.DepthFunc)
				{
				case EDepthComparissonMode::None:
					return D3D11_COMPARISON_NEVER;
				case EDepthComparissonMode::Less:
					return D3D11_COMPARISON_LESS;
				case EDepthComparissonMode::LEqual:
					return D3D11_COMPARISON_LESS_EQUAL;
				case EDepthComparissonMode::Greater:
					return D3D11_COMPARISON_GREATER;
				case EDepthComparissonMode::GEqual:
					return D3D11_COMPARISON_GREATER_EQUAL;
				case EDepthComparissonMode::Equal:
					return D3D11_COMPARISON_EQUAL;
				case EDepthComparissonMode::Always:
					return D3D11_COMPARISON_ALWAYS;
				default:
					break;
				}
				return D3D11_COMPARISON_LESS;
			});

		Desc.StencilEnable = Config.DepthStencil.bStencilEnable;
		Desc.FrontFace = std::invoke([&Config]
			{
				D3D11_DEPTH_STENCILOP_DESC Op{};
				if (Config.DepthStencil.StencilAccess & EStencilBufferAccess::Write)
				{
					Op = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
				}
				return Op;
			});

		Desc.BackFace = std::invoke([&Config]
			{
				D3D11_DEPTH_STENCILOP_DESC Op{};
				if (Config.DepthStencil.StencilAccess & EStencilBufferAccess::Write)
				{
					Op = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_DECR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
				}
				return Op;
			});
		Desc.StencilWriteMask = 0xFF;
		Desc.StencilReadMask = 0xFF;

		auto Device = CHECKED(TRenderDeviceD3D11::Get())->GetDevice();
		CHECK(Device);
		HRCHECK(Device->CreateDepthStencilState(&Desc, &DepthStencil));
	}

	void TGraphicsPipelineHandleD3D11::SetupInputLayout(RefPtr<IShader> Shader, const TGraphicsPipelineConfiguration& Config)
	{
		CHECK(IsRenderThread());
		PrimitiveTopology = std::invoke([&Config]
			{
				switch (Config.VertexInput.Topology)
				{
				case EPrimitiveTopology::TriangleList:
					return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				case EPrimitiveTopology::LineList:
					return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
				case EPrimitiveTopology::PointList:
					return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
				}
				CRASH();
			});
		
		Array<D3D11_INPUT_ELEMENT_DESC> Elements;
		Elements.Reserve(Config.VertexInput.VertexLayout.GetAttributes().GetLength());
		
		for (const VertexAttribute& Element : Config.VertexInput.VertexLayout.GetAttributes())
		{
			D3D11_INPUT_ELEMENT_DESC Desc{};
			ZeroMemory(&Desc, sizeof(Desc));
			Desc.SemanticName = Element.AttributeName.c_str();
			Desc.SemanticIndex = Element.AttributeId;
			Desc.Format = std::invoke([&Element]()->DXGI_FORMAT
				{
					switch (Element.InputType.Value)
					{
					case EShaderInputType::Float:
						return DXGI_FORMAT_R32_FLOAT;
					case EShaderInputType::Float2:
						return DXGI_FORMAT_R32G32_FLOAT;
					case EShaderInputType::Float3:
						return DXGI_FORMAT_R32G32B32_FLOAT;
					case EShaderInputType::Float4:
						return DXGI_FORMAT_R32G32B32A32_FLOAT;

					case EShaderInputType::Int:
						return DXGI_FORMAT_R32_SINT;
					case EShaderInputType::Int2:
						return DXGI_FORMAT_R32G32_SINT;
					case EShaderInputType::Int3:
						return DXGI_FORMAT_R32G32B32_SINT;
					case EShaderInputType::Int4:
						return DXGI_FORMAT_R32G32B32A32_SINT;


					case EShaderInputType::UInt:
						return DXGI_FORMAT_R32_UINT;
					case EShaderInputType::UInt2:
						return DXGI_FORMAT_R32G32_UINT;
					case EShaderInputType::UInt3:
						return DXGI_FORMAT_R32G32B32_UINT;
					case EShaderInputType::UInt4:
						return DXGI_FORMAT_R32G32B32A32_UINT;
					}
					CRASH();
					return DXGI_FORMAT_UNKNOWN;
				}
			);

			Desc.InputSlot = 0;
			Desc.AlignedByteOffset = (UINT)Element.Offset;
			Desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO: Add support for instancing
			Desc.InstanceDataStepRate = 0;
			Elements.EmplaceBack(Desc);
		}
		auto Device = CHECKED(TRenderDeviceD3D11::Get())->GetDevice();
		CHECK(Device);
		RefPtr<IAsyncDataBlob> VertexShaderBytecode = Shader->GetVertexShaderBytecode();
		HRCHECK(Device->CreateInputLayout(Elements.GetData(), (UINT)Elements.GetLength(), VertexShaderBytecode->GetData(), VertexShaderBytecode->GetSize(), &InputLayout));
	}

}