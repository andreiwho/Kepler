#include "RenderTarget.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	TRenderTarget2D::TRenderTarget2D(TRef<TImage2D> InImage, u32 MipLevl, u32 ArrayLayer)
		:	Image(InImage)
	{
	}

	TRef<TRenderTarget2D> TRenderTarget2D::New(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateRenderTarget2D(InImage, MipLevel, ArrayLayer);
	}

	//////////////////////////////////////////////////////////////////////////
	TDepthStencilTarget2D::TDepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
		:	Image(InImage)
	{
	}

	TRef<TDepthStencilTarget2D> TDepthStencilTarget2D::New(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateDepthStencilTarget2D(InImage, MipLevel, ArrayLayer);
	}

}