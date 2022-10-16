#pragma once
#include "Renderer/Elements/Buffer.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"

namespace ke
{
	class TTransferBufferD3D11 : public TTransferBuffer
	{
	public:
		TTransferBufferD3D11(usize Size, RefPtr<AsyncDataBlob> InitialData);
		~TTransferBufferD3D11();

		virtual void* GetNativeHandle() const override { return m_Buffer; }
		virtual void Write(RefPtr<GraphicsCommandListImmediate> CommandList, RefPtr<AsyncDataBlob> Data) override;
		virtual void Transfer(RefPtr<GraphicsCommandListImmediate> pImmCmd,RefPtr<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

	private:
		ID3D11Buffer* m_Buffer{ nullptr };
		RefPtr<AsyncDataBlob> m_TempDataBlob{};
	};
}