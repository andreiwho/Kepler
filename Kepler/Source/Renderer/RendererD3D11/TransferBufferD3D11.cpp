#include "TransferBufferD3D11.h"
#include "RenderDeviceD3D11.h"
#include "../RenderGlobals.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogStagingBuffer, Info);

	TTransferBufferD3D11::TTransferBufferD3D11(usize Size, RefPtr<IAsyncDataBlob> InitialData)
		: ITransferBuffer(Size, InitialData)
		, m_TempDataBlob(InitialData)
	{
		CHECK(IsRenderThread());
		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		CHECKMSG(InitialData->GetStride(), "When creating a vertex buffer using AsyncDataBlob the ElemSize of the blob must be specified");

		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = Size;
		Desc.Usage = D3D11_USAGE_STAGING;
		Desc.BindFlags = 0;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.StructureByteStride = InitialData->GetStride();

		D3D11_SUBRESOURCE_DATA BufferData{};
		ZeroMemory(&BufferData, sizeof(BufferData));
		BufferData.pSysMem = InitialData->GetData();

		D3D11_SUBRESOURCE_DATA* pBufferData = !!InitialData ? &BufferData : nullptr;
		HRCHECK(Device->CreateBuffer(&Desc, pBufferData, &m_Buffer));

		// This is used to ensure that the data buffer will live long enough on the render thread to copy the buffer data into the buffer
		m_TempDataBlob.Release();

		KEPLER_TRACE(LogStagingBuffer, "Created Staging Buffer with size {}", Size);
	}

	TTransferBufferD3D11::~TTransferBufferD3D11()
	{
		if (m_Buffer)
		{
			if (auto pDevice = TRenderDeviceD3D11::Get())
			{
				pDevice->RegisterPendingDeleteResource(m_Buffer);
			}
		}
	}

	void TTransferBufferD3D11::Write(RefPtr<class ICommandListImmediate> pImmCmd, RefPtr<IAsyncDataBlob> Data)
	{
		CHECK(IsRenderThread());
		if (m_Size < Data->GetSize())
		{
			if (m_Buffer)
			{
				if (auto pDevice = TRenderDeviceD3D11::Get())
				{
					pDevice->RegisterPendingDeleteResource(m_Buffer);
				}
			}

			new(this) TTransferBufferD3D11(Data->GetSize(), Data);
		}
		else
		{
			m_TempDataBlob = Data;
			void* Mapped = pImmCmd->MapBuffer(RefFromThis());
			memcpy(Mapped, m_TempDataBlob->GetData(), m_TempDataBlob->GetSize());
			pImmCmd->UnmapBuffer(RefFromThis());
			m_TempDataBlob.Release();
		}
	}

	void TTransferBufferD3D11::Transfer(RefPtr<ICommandListImmediate> pImmCmd, RefPtr<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size)
	{
		CHECK(IsRenderThread());
		CHECK(m_Buffer);
		pImmCmd->Transfer(RefCast<TTransferBufferD3D11>(RefFromThis()), To, DstOffset, SrcOffset, Size);
	}

}