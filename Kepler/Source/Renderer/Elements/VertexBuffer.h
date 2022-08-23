#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace Kepler
{
	class TVertexBuffer : public TBuffer
	{
	public:
		TVertexBuffer() = default;
		TVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data = nullptr);

		inline usize GetSize() const { return Size; }
		inline usize GetStride() const { return Stride; }

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};
}