#pragma once
#include "Core/Malloc.h"
#include "Renderer/RenderTypes.h"

namespace ke
{

	class TBuffer : public TEnableRefFromThis<TBuffer>
	{
	public:
		virtual void* GetNativeHandle() const = 0;
	};

	class TTransferBuffer : public TBuffer
	{
	protected:
		TTransferBuffer(usize Size, TRef<TDataBlob> InitialData = nullptr);

	public:
		static TRef<TTransferBuffer> New(usize Size, TRef<TDataBlob> InitialData = nullptr);
		virtual void Write(TRef<class TCommandListImmediate> CommandList, TRef<TDataBlob> Data) = 0;
		virtual void Transfer(TRef<TCommandListImmediate> pImmCmd, TRef<TBuffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		inline usize GetSize() const { return Size; }

	protected:
		usize Size = 0;
	};
}