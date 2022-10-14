#pragma once
#include "Renderer/Elements/Buffer.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"

namespace ke
{
	class TTransferBufferD3D11 : public TTransferBuffer
	{
	public:
		TTransferBufferD3D11(usize Size, TRef<AsyncDataBlob> InitialData);
		~TTransferBufferD3D11();

		virtual void* GetNativeHandle() const override { return m_Buffer; }
		virtual void Write(TRef<GraphicsCommandListImmediate> CommandList, TRef<AsyncDataBlob> Data) override;
		virtual void Transfer(TRef<GraphicsCommandListImmediate> pImmCmd,TRef<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

	private:
		ID3D11Buffer* m_Buffer{ nullptr };
		TRef<AsyncDataBlob> m_TempDataBlob{};
	};
}