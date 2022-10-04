#pragma once
#include "Renderer/Elements/IndexBuffer.h"
#include "D3D11Common.h"

namespace ke
{
	class TIndexBufferD3D11 : public TIndexBuffer
	{
	public:
		TIndexBufferD3D11() = default;
		TIndexBufferD3D11(EBufferAccessFlags InAccess, TRef<AsyncDataBlob> Data);
		~TIndexBufferD3D11();
		
		inline ID3D11Buffer* GetBuffer() const { return Buffer; }
		virtual void* GetNativeHandle() const override { return Buffer; }

	private:
		ID3D11Buffer* Buffer{};
		TRef<AsyncDataBlob> TempDataBlob{};
	};
}