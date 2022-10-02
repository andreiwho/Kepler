#pragma once
#include "Renderer/Elements/VertexBuffer.h"
#include "D3D11Common.h"

namespace ke
{
	class TVertexBufferD3D11 : public TVertexBuffer
	{
	public:
		TVertexBufferD3D11() = default;
		TVertexBufferD3D11(EBufferAccessFlags InAccess, TRef<TDataBlob> Data);
		~TVertexBufferD3D11();
		
		inline ID3D11Buffer* GetBuffer() const { return Buffer; }
		virtual void* GetNativeHandle() const override { return Buffer; }

	private:
		ID3D11Buffer* Buffer{};
		TRef<TDataBlob> TempDataBlob{};
	};
}