#include "RenderTargetD3D11.h"
#include "Core/Macros.h"
#include "Renderer/RenderGlobals.h"
#include "ImageD3D11.h"
#include "RenderDeviceD3D11.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TRenderTarget2D_D3D11::TRenderTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
		:	TRenderTarget2D(InImage, MipLevel, ArrayLayer)
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
			HRCHECK(Device->GetDevice()->CreateRenderTargetView(Texture, &Desc, &RenderTarget));
		}
	}

	TRenderTarget2D_D3D11::~TRenderTarget2D_D3D11()
	{
		if (!RenderTarget)
		{
			return;
		}

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(RenderTarget);
		}
	}


	//////////////////////////////////////////////////////////////////////////
	TDepthStencilTarget2D_D3D11::TDepthStencilTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer) 
		:	TDepthStencilTarget2D(InImage, MipLevel, ArrayLayer)
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
			HRCHECK(Device->GetDevice()->CreateDepthStencilView(Texture, &Desc, &View));
		}
	}

	TDepthStencilTarget2D_D3D11::~TDepthStencilTarget2D_D3D11()
	{
		if (!View)
		{
			return;
		}

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(View);
		}
	}

}