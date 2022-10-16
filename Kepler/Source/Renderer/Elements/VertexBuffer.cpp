#include "VertexBuffer.h"
#include "../RenderGlobals.h"
#include "../RenderDevice.h"

namespace ke
{

	IVertexBuffer::IVertexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<IAsyncDataBlob> Data)
		: m_AccessFlags(InAccessFlags)
		, m_Size(Data ? Data->GetSize() : 0)
		, m_Stride(Data ? Data->GetStride() : 0)
	{
	}

	RefPtr<IVertexBuffer> IVertexBuffer::New(EBufferAccessFlags InAccessFlags, RefPtr<IAsyncDataBlob> Data)
	{
		return GetRenderDevice()->CreateVertexBuffer(InAccessFlags, Data);
	}

	IVertexBufferDynamic::IVertexBufferDynamic(EBufferAccessFlags accessFlags, usize size, usize stride)
		:	m_AccessFlags(accessFlags)
		,	m_Size(size)
		,	m_Stride(stride)

	{
	}

	RefPtr<IVertexBufferDynamic> IVertexBufferDynamic::New(EBufferAccessFlags accessFlags, usize size, usize stride)
	{
		return GetRenderDevice()->CreateDynamicVertexBuffer(accessFlags, size, stride);
	}

}