#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace ke
{
	class TVertexBuffer : public IBuffer
	{
	protected:
		TVertexBuffer() = default;
		TVertexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data = nullptr);
	
	public:
		inline usize GetSize() const { return Size; }
		inline usize GetStride() const { return Stride; }

		static RefPtr<TVertexBuffer> New(EBufferAccessFlags InAccessFlags, RefPtr<AsyncDataBlob> Data = nullptr);

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};

	class DynamicVertexBuffer : public IBuffer
	{
	protected:
		DynamicVertexBuffer() = default;
		DynamicVertexBuffer(EBufferAccessFlags accessFlags, usize size, usize stride);
		
	public:
		inline usize GetSize() const { return m_Size; }
		inline usize GetStride() const { return m_Stride; }
		
		static RefPtr<DynamicVertexBuffer> New(EBufferAccessFlags accessFlags, usize size, usize stride);
		// Resize and invalidate buffer contents (note: buffer data should be rewritten)
		virtual void RT_Resize(usize size) = 0;

	protected:
		EBufferAccessFlags m_AccessFlags{};
		usize m_Size{};
		usize m_Stride{};
	};
}