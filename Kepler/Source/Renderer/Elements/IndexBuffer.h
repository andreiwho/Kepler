#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace ke
{
	class TIndexBuffer : public IBuffer
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

	class DynamicIndexBuffer : public IBuffer
	{
	protected:
		DynamicIndexBuffer() = default;
		DynamicIndexBuffer(EBufferAccessFlags accessFlags, usize size, usize stride);

	public:
		inline usize GetSize() const { return m_Size; }
		inline usize GetStride() const { return m_Stride; }

		static TRef<DynamicIndexBuffer> New(EBufferAccessFlags accessFlags, usize size, usize stride);
		// Resize and invalidate buffer contents (note: buffer data should be rewritten)
		virtual void RT_Resize(usize size) = 0;

	protected:
		EBufferAccessFlags m_AccessFlags{};
		usize m_Size{};
		usize m_Stride{};
	};
}