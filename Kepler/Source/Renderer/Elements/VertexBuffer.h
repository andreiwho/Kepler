#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include "Buffer.h"

namespace ke
{
	class IVertexBuffer : public IBuffer
	{
	protected:
		IVertexBuffer() = default;
		IVertexBuffer(EBufferAccessFlags accessFlags, RefPtr<IAsyncDataBlob> pData = nullptr);
	
	public:
		inline usize GetSize() const { return m_Size; }
		inline usize GetStride() const { return m_Stride; }

		static RefPtr<IVertexBuffer> New(EBufferAccessFlags accessFlags, RefPtr<IAsyncDataBlob> pData = nullptr);

	protected:
		EBufferAccessFlags m_AccessFlags{};
		usize m_Size{};
		usize m_Stride{};
	};

	class IVertexBufferDynamic : public IBuffer
	{
	protected:
		IVertexBufferDynamic() = default;
		IVertexBufferDynamic(EBufferAccessFlags accessFlags, usize size, usize stride);
		
	public:
		inline usize GetSize() const { return m_Size; }
		inline usize GetStride() const { return m_Stride; }
		
		static RefPtr<IVertexBufferDynamic> New(EBufferAccessFlags accessFlags, usize size, usize stride);
		// Resize and invalidate buffer contents (note: buffer data should be rewritten)
		virtual void RT_Resize(usize size) = 0;

	protected:
		EBufferAccessFlags m_AccessFlags{};
		usize m_Size{};
		usize m_Stride{};
	};
}