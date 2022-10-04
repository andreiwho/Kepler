#pragma once
#include "Core/Malloc.h"
#include "Renderer/RenderTypes.h"

namespace ke
{

	class Buffer : public TEnableRefFromThis<Buffer>
	{
	public:
		virtual void* GetNativeHandle() const = 0;
	};

	class TTransferBuffer : public Buffer
	{
	protected:
		TTransferBuffer(usize Size, TRef<AsyncDataBlob> InitialData = nullptr);

	public:
		static TRef<TTransferBuffer> New(usize Size, TRef<AsyncDataBlob> InitialData = nullptr);
		virtual void Write(TRef<class GraphicsCommandListImmediate> CommandList, TRef<AsyncDataBlob> Data) = 0;
		virtual void Transfer(TRef<GraphicsCommandListImmediate> pImmCmd, TRef<Buffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		inline usize GetSize() const { return Size; }

	protected:
		usize Size = 0;
	};
}