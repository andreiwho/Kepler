#pragma once
#include "Buffer.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/RenderDevice.h"

namespace ke
{
	ITransferBuffer::ITransferBuffer(usize InSize, RefPtr<IAsyncDataBlob> InitialData)
		:	m_Size(InSize)
	{
	}

	RefPtr<ITransferBuffer> ITransferBuffer::New(usize size, RefPtr<IAsyncDataBlob> pData)
	{
		return GetRenderDevice()->CreateTransferBuffer(size, pData);
	}
}