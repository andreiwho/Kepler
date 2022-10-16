#pragma once
#include "Core/Malloc.h"
#include "Renderer/RenderTypes.h"

namespace ke
{

	class IBuffer : public TEnableRefFromThis<IBuffer>
	{
	public:
		virtual void* GetNativeHandle() const = 0;
	};

	class TTransferBuffer : public IBuffer
	{
	protected:
		TTransferBuffer(usize Size, RefPtr<AsyncDataBlob> InitialData = nullptr);

	public:
		static RefPtr<TTransferBuffer> New(usize Size, RefPtr<AsyncDataBlob> InitialData = nullptr);
		virtual void Write(RefPtr<class GraphicsCommandListImmediate> CommandList, RefPtr<AsyncDataBlob> Data) = 0;
		virtual void Transfer(RefPtr<GraphicsCommandListImmediate> pImmCmd, RefPtr<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		inline usize GetSize() const { return Size; }

	protected:
		usize Size = 0;
	};
}