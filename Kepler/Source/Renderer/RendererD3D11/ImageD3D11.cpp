#include "ImageD3D11.h"
#include "RenderDeviceD3D11.h"
#include "../RenderGlobals.h"
#include "CommandListImmediateD3D11.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	// IMAGE2D
	//////////////////////////////////////////////////////////////////////////
	TImage2D_D3D11::TImage2D_D3D11(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: IImage2D(InWidth, InHeight, InFormat, InUsage, InMipLevels, InArraySize)
	{
		CD3D11_TEXTURE2D_DESC Desc;
		Desc.Width = m_Width;
		Desc.Height = m_Height;
		Desc.MipLevels = InMipLevels;
		Desc.ArraySize = InArraySize;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.Format = static_cast<DXGI_FORMAT>(InFormat.Value);
		Desc.BindFlags = 0;
		Desc.SampleDesc = { 1, 0 };

		if (m_Usage & EImageUsage::ShaderResource)
		{
			Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (m_Usage & EImageUsage::DepthTarget)
		{
			Desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		}
		if (m_Usage & EImageUsage::RenderTarget)
		{
			Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}

		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = 0;

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			HRCHECK(Device->GetDevice()->CreateTexture2D(&Desc, nullptr, &Image));

			if (m_Usage & EImageUsage::AllowCPURead)
			{
				// Create readback image	
				Desc.Usage = D3D11_USAGE_STAGING;
				Desc.BindFlags = 0;
				Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				HRCHECK(Device->GetDevice()->CreateTexture2D(&Desc, nullptr, &ReadbackImage));
			}
		}
	}

	TImage2D_D3D11::~TImage2D_D3D11()
	{
		if (!Image)
		{
			return;
		}

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		if (Device)
		{
			Device->RegisterPendingDeleteResource(Image);

			if (ReadbackImage)
			{
				Device->RegisterPendingDeleteResource(ReadbackImage);
			}
		}
	}

	void TImage2D_D3D11::RequireReadbackCopy(RefPtr<class ICommandListImmediate> pImmCtx)
	{
		auto MyCtx = RefCast<GraphicsCommandListImmediateD3D11>(pImmCtx);
		ID3D11DeviceContext4* pCtx = MyCtx->GetContext();
		pCtx->CopyResource(ReadbackImage, Image);
	}
}