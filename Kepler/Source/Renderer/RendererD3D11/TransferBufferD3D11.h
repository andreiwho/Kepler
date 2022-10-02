#pragma once
#include "Renderer/Elements/Buffer.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"

namespace ke
{
	class TTransferBufferD3D11 : public TTransferBuffer
	{
	public:
		TTransferBufferD3D11(usize Size, TRef<TDataBlob> InitialData);
		~TTransferBufferD3D11();

		virtual void* GetNativeHandle() const override { return Buffer; }
		virtual void Write(TRef<TCommandListImmediate> CommandList, TRef<TDataBlob> Data) override;
		virtual void Transfer(TRef<TCommandListImmediate> pImmCmd,TRef<TBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

	private:
		ID3D11Buffer* Buffer{ nullptr };
		TRef<TDataBlob> TempDataBlob{};
	};
}