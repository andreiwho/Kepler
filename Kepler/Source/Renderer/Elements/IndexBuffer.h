#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace ke
{
	class TIndexBuffer : public Buffer
	{
	protected:
		TIndexBuffer() = default;
		TIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data = nullptr);
	
	public:
		inline usize GetSize() const { return Size; }
		inline usize GetStride() const { return Stride; }
		inline usize GetCount() const { return Size / Stride; }

		static TRef<TIndexBuffer> New(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data = nullptr);

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};
}