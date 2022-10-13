#pragma once
#include "Renderer/Elements/VertexBuffer.h"
#include "D3D11Common.h"

namespace ke
{
	class TVertexBufferD3D11 : public TVertexBuffer
	{
	public:
		TVertexBufferD3D11() = default;
		TVertexBufferD3D11(EBufferAccessFlags InAccess, TRef<AsyncDataBlob> Data);
		~TVertexBufferD3D11();
		
		inline ID3D11Buffer* GetBuffer() const { return Buffer; }
		virtual void* GetNativeHandle() const override { return Buffer; }

	private:
		ID3D11Buffer* Buffer{};
		TRef<AsyncDataBlob> TempDataBlob{};
	};

	class DynamicVertexBufferD3D11 : public DynamicVertexBuffer
	{
	public:
		DynamicVertexBufferD3D11() = default;
		DynamicVertexBufferD3D11(EBufferAccessFlags flags, usize size, usize stride);
		~DynamicVertexBufferD3D11();

		inline ID3D11Buffer* GetBuffer() const { return m_Buffer; }
		virtual void* GetNativeHandle() const override { return m_Buffer; }

		virtual void RT_Resize(usize size) override;

	private:
		ID3D11Buffer* m_Buffer;
	};
}