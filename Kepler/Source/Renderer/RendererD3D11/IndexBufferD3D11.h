#pragma once
#include "Renderer/Elements/IndexBuffer.h"
#include "D3D11Common.h"

namespace Kepler
{
	class TIndexBufferD3D11 : public TIndexBuffer
	{
	public:
		TIndexBufferD3D11() = default;
		TIndexBufferD3D11(EBufferAccessFlags InAccess, TRef<TDataBlob> Data);
		~TIndexBufferD3D11();
		
		inline ID3D11Buffer* GetBuffer() const { return Buffer; }

	private:
		ID3D11Buffer* Buffer{};
		TRef<TDataBlob> TempDataBlob{};
	};
}