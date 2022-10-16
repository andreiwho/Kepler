#include "VertexBuffer.h"
#include "../RenderGlobals.h"
#include "../RenderDevice.h"

namespace ke
{

	TVertexBuffer::TVertexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}

	RefPtr<TVertexBuffer> TVertexBuffer::New(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data)
	{
		return GetRenderDevice()->CreateVertexBuffer(InAccessFlags, Data);
	}

	DynamicVertexBuffer::DynamicVertexBuffer(EBufferAccessFlags accessFlags, usize size, usize stride)
		:	m_AccessFlags(accessFlags)
		,	m_Size(size)
		,	m_Stride(stride)

	{
	}

	RefPtr<DynamicVertexBuffer> DynamicVertexBuffer::New(EBufferAccessFlags accessFlags, usize size, usize stride)
	{
		return GetRenderDevice()->CreateDynamicVertexBuffer(accessFlags, size, stride);
	}

}