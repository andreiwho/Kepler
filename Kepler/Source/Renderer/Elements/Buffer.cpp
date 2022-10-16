#pragma once
#include "Buffer.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	TTransferBuffer::TTransferBuffer(usize InSize, RefPtr<AsyncDataBlob> InitialData)
		:	Size(InSize)
	{
	}

	RefPtr<TTransferBuffer> TTransferBuffer::New(usize Size, RefPtr<AsyncDataBlob> InitialData)
	{
		return GetRenderDevice()->CreateTransferBuffer(Size, InitialData);
	}
}