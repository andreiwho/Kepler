#include "RenderTarget.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	RenderTarget2D::RenderTarget2D(RefPtr<TImage2D> InImage, u32 MipLevl, u32 ArrayLayer)
		:	m_Image(InImage)
	{
	}

	RefPtr<RenderTarget2D> RenderTarget2D::New(RefPtr<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateRenderTarget2D(InImage, MipLevel, ArrayLayer);
	}

	//////////////////////////////////////////////////////////////////////////
	DepthStencilTarget2D::DepthStencilTarget2D(RefPtr<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer, bool bReadOnly)
		:	m_Image(InImage)
	{
	}

	RefPtr<DepthStencilTarget2D> DepthStencilTarget2D::New(RefPtr<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(InImage, MipLevel, ArrayLayer);
	}

	RefPtr<DepthStencilTarget2D> DepthStencilTarget2D::NewReadOnly(RefPtr<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(InImage, MipLevel, ArrayLayer, true);
	}

}