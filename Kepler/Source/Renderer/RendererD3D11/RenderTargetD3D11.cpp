#include "RenderTargetD3D11.h"
#include "Core/Macros.h"
#include "Renderer/RenderGlobals.h"
#include "ImageD3D11.h"
#include "RenderDeviceD3D11.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	RenderTarget2D_D3D11::RenderTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
		:	RenderTarget2D(InImage, MipLevel, ArrayLayer)
	{
		CHECK(IsRenderThread());

		TRef<TImage2D_D3D11> MyImage = RefCast<TImage2D_D3D11>(InImage);
		CHECKMSG(MyImage, "Attempted to create a render target from null image");
		ID3D11Texture2D* Texture = MyImage->GetImage();
		CHECK(Texture);

		CD3D11_RENDER_TARGET_VIEW_DESC Desc(Texture, D3D11_RTV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)MyImage->GetFormat().Value, MipLevel, ArrayLayer, 1);
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			HRCHECK(Device->GetDevice()->CreateRenderTargetView(Texture, &Desc, &m_RenderTarget));
		}
	}

	RenderTarget2D_D3D11::~RenderTarget2D_D3D11()
	{
		if (!m_RenderTarget)
		{
			return;
		}

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(m_RenderTarget);
		}
	}


	//////////////////////////////////////////////////////////////////////////
	DepthStencilTarget2D_D3D11::DepthStencilTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer) 
		:	DepthStencilTarget2D(InImage, MipLevel, ArrayLayer)
	{
		CHECK(IsRenderThread());

		TRef<TImage2D_D3D11> MyImage = RefCast<TImage2D_D3D11>(InImage);
		CHECKMSG(MyImage, "Attempted to create a depth stencil target from null image");
		ID3D11Texture2D* Texture = MyImage->GetImage();
		CHECK(Texture);

		CD3D11_DEPTH_STENCIL_VIEW_DESC Desc(Texture, D3D11_DSV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)MyImage->GetFormat().Value, ArrayLayer, 1);
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			HRCHECK(Device->GetDevice()->CreateDepthStencilView(Texture, &Desc, &m_View));
		}
	}

	DepthStencilTarget2D_D3D11::~DepthStencilTarget2D_D3D11()
	{
		if (!m_View)
		{
			return;
		}

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(m_View);
		}
	}

}