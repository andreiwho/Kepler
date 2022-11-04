#pragma once
#include "Core/Malloc.h"
#include "Renderer/RenderTypes.h"

namespace ke
{

	class IBuffer : public EnableRefPtrFromThis<IBuffer>
	{
	public:
		virtual void* GetNativeHandle() const = 0;
	};

	class ITransferBuffer : public IBuffer
	{
	protected:
		ITransferBuffer(usize size, RefPtr<IAsyncDataBlob> pData = nullptr);

	public:
		static RefPtr<ITransferBuffer> New(usize size, RefPtr<IAsyncDataBlob> pData = nullptr);
		virtual void Write(RefPtr<class ICommandListImmediate> pImmCtx, RefPtr<IAsyncDataBlob> pData) = 0;
		virtual void Transfer(RefPtr<ICommandListImmediate> pImmCmd, RefPtr<IBuffer> pTo, usize dstOffset, usize srcOffset, usize size) = 0;

		inline usize GetSize() const { return m_Size; }

	protected:
		usize m_Size = 0;
	};
}