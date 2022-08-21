#include "GraphicsPipelineHandleD3D11.h"
#include "Renderer/RendererD3D11/RenderDeviceD3D11.h"
#include "../RenderGlobals.h"

namespace Kepler
{

	TGraphicsPipelineHandleD3D11::TGraphicsPipelineHandleD3D11(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config)
	{
		SetupRasterizer(Config);
		SetupDepthStencil(Config);
		SetupInputLayout(Shader);
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

		auto Device = CHECKED(TRenderDeviceD3D11::Get())->GetDevice();
		CHECK(Device);
		HRCHECK(Device->CreateDepthStencilState(&Desc, &DepthStencil));
	}

	void TGraphicsPipelineHandleD3D11::SetupInputLayout(TRef<TShader> Shader)
	{

	}

}