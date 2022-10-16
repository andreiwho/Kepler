#pragma once
#include "Renderer/Elements/IndexBuffer.h"
#include "D3D11Common.h"

namespace ke
{
	class TIndexBufferD3D11 : public TIndexBuffer
	{
	public:
		TIndexBufferD3D11() = default;
		TIndexBufferD3D11(EBufferAccessFlags InAccess, RefPtr<AsyncDataBlob> Data);
		~TIndexBufferD3D11();
		
		inline ID3D11Buffer* GetBuffer() const { return Buffer; }
		virtual void* GetNativeHandle() const override { return Buffer; }

	private:
		ID3D11Buffer* Buffer{};
		RefPtr<AsyncDataBlob> TempDataBlob{};
	};

	class DynamicIndexBufferD3D11 : public DynamicIndexBuffer
	{
	public:
		DynamicIndexBufferD3D11() = default;
		DynamicIndexBufferD3D11(EBufferAccessFlags flags, usize size, usize stride);
		~DynamicIndexBufferD3D11();

		inline ID3D11Buffer* GetBuffer() const { return m_Buffer; }
		virtual void* GetNativeHandle() const override { return m_Buffer; }

		virtual void RT_Resize(usize size) override;

	private:
		ID3D11Buffer* m_Buffer{};
	};
}