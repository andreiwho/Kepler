#include "Image.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"
#include "../RenderDevice.h"
#include "CommandList.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	TImage::TImage(EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		:	Format(InFormat)
		,	Usage(InUsage)
		,	MipLevels(InMipLevels)
		,	ArraySize(InArraySize)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TImage2D::TImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
		, Height(InHeight)
	{
	}

	void TImage2D::Write(TRef<GraphicsCommandListImmediate> pImmCmd, usize X, usize Y, usize Width, usize Height, TRef<AsyncDataBlob> Data)
	{
		CHECK(IsRenderThread());
		if (pImmCmd)
		{
			pImmCmd->Transfer(RefCast<TImage2D>(RefFromThis()), X, Y, Width, Height, Data);
		}
	}

	TRef<TImage2D> TImage2D::New(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
	{
		return GetRenderDevice()->CreateImage2D(InWidth, InHeight, InFormat, InUsage, InMipLevels, InArraySize);
	}
}