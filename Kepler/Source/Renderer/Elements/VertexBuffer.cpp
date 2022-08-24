#include "VertexBuffer.h"

namespace Kepler
{

	TVertexBuffer::TVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}

}