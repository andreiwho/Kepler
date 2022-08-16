#include "IndexBuffer.h"

namespace Kepler
{
	TIndexBuffer::TIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
		: AccessFlags(InAccessFlags)
		, Size(Data ? Data->GetSize() : 0)
		, Stride(Data ? Data->GetStride() : 0)
	{
	}
}