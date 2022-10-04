#pragma once
#include "Buffer.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	TTransferBuffer::TTransferBuffer(usize InSize, TRef<AsyncDataBlob> InitialData)
		:	Size(InSize)
	{
	}

	TRef<TTransferBuffer> TTransferBuffer::New(usize Size, TRef<AsyncDataBlob> InitialData)
	{
		return GetRenderDevice()->CreateTransferBuffer(Size, InitialData);
	}

}