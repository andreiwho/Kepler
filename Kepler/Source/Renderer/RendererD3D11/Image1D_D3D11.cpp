#include "Image1D_D3D11.h"
#include "RenderDeviceD3D11.h"
#include "../RenderGlobals.h"

namespace Kepler
{
	TImage1D_D3D11::TImage1D_D3D11(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		:	TImage1D(InWidth, InFormat, InUsage, InMipLevels, InArraySize)
	{
		CD3D11_TEXTURE1D_DESC Desc;
		Desc.Width = Width;
		Desc.MipLevels = InMipLevels;
		Desc.ArraySize = InArraySize;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.Format = static_cast<DXGI_FORMAT>(InFormat.Value);

		if (Usage & EImageUsage::ShaderResource)
		{
			Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (Usage & EImageUsage::DepthTarget)
		{
			Desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		}
		if (Usage & EImageUsage::RenderTarget)
		{
			Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}

		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = 0;

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			HRCHECK(Device->GetDevice()->CreateTexture1D(&Desc, nullptr, &Image));
		}
	}

	TImage1D_D3D11::~TImage1D_D3D11()
	{
		if (!Image)
		{
			return;
		}
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(Image);
		}
	}
}