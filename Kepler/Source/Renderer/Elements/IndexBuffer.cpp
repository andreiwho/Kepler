#include "IndexBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace ke
{
	IIndexBuffer::IIndexBuffer(EBufferAccessFlags accessFlags, RefPtr<IAsyncDataBlob> pData)
		: m_AccessFlags(accessFlags)
		, m_Size(pData ? pData->GetSize() : 0)
		, m_Stride(pData ? pData->GetStride() : 0)
	{
	}

	RefPtr<IIndexBuffer> IIndexBuffer::New(EBufferAccessFlags accessFlags, RefPtr<IAsyncDataBlob> pData)
	{
		return GetRenderDevice()->CreateIndexBuffer(accessFlags, pData);
	}

	IIndexBufferDynamic::IIndexBufferDynamic(EBufferAccessFlags accessFlags, usize size, usize stride)
		:	m_AccessFlags(accessFlags)
		,	m_Size(size)
		,	m_Stride(stride)
	{
	}

	RefPtr<IIndexBufferDynamic> IIndexBufferDynamic::New(EBufferAccessFlags accessFlags, usize size, usize stride)
	{
		return GetRenderDevice()->CreateDynamicIndexBuffer(accessFlags, size, stride);
	}

}