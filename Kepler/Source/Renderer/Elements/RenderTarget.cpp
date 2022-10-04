#include "RenderTarget.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	RenderTarget2D::RenderTarget2D(TRef<TImage2D> InImage, u32 MipLevl, u32 ArrayLayer)
		:	m_Image(InImage)
	{
	}

	TRef<RenderTarget2D> RenderTarget2D::New(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateRenderTarget2D(InImage, MipLevel, ArrayLayer);
	}

	//////////////////////////////////////////////////////////////////////////
	DepthStencilTarget2D::DepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
		:	m_Image(InImage)
	{
	}

	TRef<DepthStencilTarget2D> DepthStencilTarget2D::New(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(InImage, MipLevel, ArrayLayer);
	}

}