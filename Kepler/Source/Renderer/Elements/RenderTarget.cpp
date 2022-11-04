#include "RenderTarget.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	IRenderTarget2D::IRenderTarget2D(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer)
		:	m_Image(pImage)
	{
	}

	RefPtr<IRenderTarget2D> IRenderTarget2D::New(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer)
	{
		return GetRenderDevice()->CreateRenderTarget2D(pImage, mipLevel, arrayLayer);
	}

	//////////////////////////////////////////////////////////////////////////
	IDepthStencilTarget2D::IDepthStencilTarget2D(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer, bool bReadOnly)
		:	m_Image(pImage)
	{
	}

	RefPtr<IDepthStencilTarget2D> IDepthStencilTarget2D::New(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(pImage, mipLevel, arrayLayer);
	}

	RefPtr<IDepthStencilTarget2D> IDepthStencilTarget2D::NewReadOnly(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(pImage, mipLevel, arrayLayer, true);
	}

}