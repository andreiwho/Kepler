#include "Image.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"
#include "../RenderDevice.h"

namespace Kepler
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
	TImage1D::TImage1D(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
	{
	}

	TRef<TImage1D> TImage1D::New(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
	{
		return GetRenderDevice()->CreateImage1D(InWidth, InFormat, InUsage, InMipLevels, InArraySize);
	}

	//////////////////////////////////////////////////////////////////////////
	TImage2D::TImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
		, Height(InHeight)
	{
	}

	TRef<TImage2D> TImage2D::New(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
	{
		return GetRenderDevice()->CreateImage2D(InWidth, InHeight, InFormat, InUsage, InMipLevels, InArraySize);
	}

	//////////////////////////////////////////////////////////////////////////
	TImage3D::TImage3D(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
		, Height(InHeight)
		, Depth(InDepth)
	{
	}

	TRef<TImage3D> TImage3D::New(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
	{
		return GetRenderDevice()->CreateImage3D(InWidth, InHeight, InDepth, InFormat, InUsage, InMipLevels, InArraySize);
	}
}