#include "Image.h"

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

	//////////////////////////////////////////////////////////////////////////
	TImage2D::TImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
		, Height(InHeight)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TImage3D::TImage3D(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: TImage(InFormat, InUsage, InMipLevels, ArraySize)
		, Width(InWidth)
		, Height(InHeight)
		, Depth(InDepth)
	{
	}
}