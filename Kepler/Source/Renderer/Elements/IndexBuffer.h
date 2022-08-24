#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	class TIndexBuffer : public TRefCounted
	{
	public:
		TIndexBuffer() = default;
		TIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data = nullptr);

		inline usize GetSize() const { return Size; }
		inline usize GetStride() const { return Stride; }
		inline usize GetCount() const { return Size / Stride; }

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};
}