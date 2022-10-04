#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace ke
{
	class TVertexBuffer : public Buffer
	{
	protected:
		TVertexBuffer() = default;
		TVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data = nullptr);
	
	public:
		inline usize GetSize() const { return Size; }
		inline usize GetStride() const { return Stride; }

		static TRef<TVertexBuffer> New(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data = nullptr);

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};
}