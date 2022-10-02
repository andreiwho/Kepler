#include "IndexBufferD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Core/Log.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogIndexBuffer);

	TIndexBufferD3D11::TIndexBufferD3D11(EBufferAccessFlags InAccess, TRef<TDataBlob> Data)
		:	TIndexBuffer(InAccess, Data), TempDataBlob(Data)
	{
		if (!Data)
		{
			return;
		}

		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		CHECKMSG(Data->GetStride(), "When creating a index buffer using TDataBlob the ElemSize of the blob must be specified");

		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = Size;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		Desc.CPUAccessFlags = 0;

		if (InAccess & EBufferAccessFlags::ReadAccess)
		{
			Desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		}

		if (InAccess & EBufferAccessFlags::WriteAccess)
		{
			Desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}

		if (InAccess & EBufferAccessFlags::GPUOnly)
		{
			if (Desc.CPUAccessFlags != 0)
			{
				KEPLER_WARNING(LogIndexBuffer, "You cannot mix EBufferUsageFlags::GPUOnly with other ones." 
					" If using GPUOnly, other flags will be ignored.");
			}

			Desc.CPUAccessFlags = 0;
		}

		Desc.StructureByteStride = Data->GetStride();

		D3D11_SUBRESOURCE_DATA BufferData{};
		ZeroMemory(&BufferData, sizeof(BufferData));
		BufferData.pSysMem = Data->GetData();
		
		HRCHECK(Device->CreateBuffer(&Desc, &BufferData, &Buffer));

		// This is used to ensure that the data buffer will live long enough on the render thread to copy the buffer data into the buffer
		TempDataBlob.Release();

		KEPLER_TRACE(LogIndexBuffer, "Created idx Buffer with size {} and stride {} and count {}", Size, Stride, GetCount());
	}

	TIndexBufferD3D11::~TIndexBufferD3D11()
	{
		if (TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get())
		{
			Device->RegisterPendingDeleteResource(Buffer);
		}
	}

}