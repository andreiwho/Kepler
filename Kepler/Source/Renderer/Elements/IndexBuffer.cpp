#include "IndexBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace ke
{
	TIndexBuffer::TIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}

	TRef<TIndexBuffer> TIndexBuffer::New(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data)
	{
		return GetRenderDevice()->CreateIndexBuffer(InAccessFlags, Data);
	}

	DynamicIndexBuffer::DynamicIndexBuffer(EBufferAccessFlags accessFlags, usize size, usize stride)
		:	m_AccessFlags(accessFlags)
		,	m_Size(size)
		,	m_Stride(stride)
	{
	}

	TRef<DynamicIndexBuffer> DynamicIndexBuffer::New(EBufferAccessFlags accessFlags, usize size, usize stride)
	{
		return GetRenderDevice()->CreateDynamicIndexBuffer(accessFlags, size, stride);
	}

}