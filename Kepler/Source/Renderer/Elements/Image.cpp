#include "Image.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"
#include "../RenderDevice.h"
#include "CommandList.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	IImage::IImage(EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		:	m_Format(InFormat)
		,	m_Usage(InUsage)
		,	m_MipLevels(InMipLevels)
		,	m_ArraySize(InArraySize)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	IImage2D::IImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels, u32 InArraySize)
		: IImage(InFormat, InUsage, InMipLevels, m_ArraySize)
		, m_Width(InWidth)
		, m_Height(InHeight)
	{
	}

	void IImage2D::Write(RefPtr<ICommandListImmediate> pImmCmd, usize x, usize y, usize width, usize height, RefPtr<IAsyncDataBlob> pData)
	{
		CHECK(IsRenderThread());
		if (pImmCmd)
		{
			pImmCmd->Transfer(RefCast<IImage2D>(RefFromThis()), x, y, width, height, pData);
		}
	}

	RefPtr<IImage2D> IImage2D::New(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mipLevels, u32 arraySize)
	{
		return GetRenderDevice()->CreateImage2D(width, height, format, usage, mipLevels, arraySize);
	}
}