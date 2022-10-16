#include "IndexBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace ke
{
	TIndexBuffer::TIndexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}

	RefPtr<TIndexBuffer> TIndexBuffer::New(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data)
	{
		return GetRenderDevice()->CreateIndexBuffer(InAccessFlags, Data);
	}

	DynamicIndexBuffer::DynamicIndexBuffer(EBufferAccessFlags accessFlags, usize size, usize stride)
		:	m_AccessFlags(accessFlags)
		,	m_Size(size)
		,	m_Stride(stride)
	{
	}

	RefPtr<DynamicIndexBuffer> DynamicIndexBuffer::New(EBufferAccessFlags accessFlags, usize size, usize stride)
	{
		return GetRenderDevice()->CreateDynamicIndexBuffer(accessFlags, size, stride);
	}

}