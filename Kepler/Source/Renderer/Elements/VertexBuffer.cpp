#include "VertexBuffer.h"
#include "../RenderGlobals.h"
#include "../RenderDevice.h"

namespace Kepler
{

	TVertexBuffer::TVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}

	TRef<TVertexBuffer> TVertexBuffer::New(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
	{
		return GetRenderDevice()->CreateVertexBuffer(InAccessFlags, Data);
	}

}