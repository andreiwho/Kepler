#pragma once
#include "Buffer.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	TTransferBuffer::TTransferBuffer(usize InSize, TRef<TDataBlob> InitialData)
		:	Size(InSize)
	{
	}

	TRef<TTransferBuffer> TTransferBuffer::New(usize Size, TRef<TDataBlob> InitialData)
	{
		return GetRenderDevice()->CreateTransferBuffer(Size, InitialData);
	}

}