#include "VertexBufferD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Core/Log.h"
#include "../RenderGlobals.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogVertexBuffer, Info);

	TVertexBufferD3D11::TVertexBufferD3D11(EBufferAccessFlags InAccess, TRef<AsyncDataBlob> Data)
		: TVertexBuffer(InAccess, Data), TempDataBlob(Data)
	{
		CHECK(Data);

		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		CHECKMSG(Data->GetStride(), "When creating a vertex buffer using AsyncDataBlob the ElemSize of the blob must be specified");

		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = Size;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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
				KEPLER_WARNING(LogVertexBuffer, "You cannot mix EBufferUsageFlags::GPUOnly with other ones."
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

		KEPLER_TRACE(LogVertexBuffer, "Created Vertex Buffer with size {} and stride {}", Size, Stride);
	}

	TVertexBufferD3D11::~TVertexBufferD3D11()
	{
		if (TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get())
		{
			Device->RegisterPendingDeleteResource(Buffer);
		}
	}

	DynamicVertexBufferD3D11::DynamicVertexBufferD3D11(EBufferAccessFlags flags, usize size, usize stride)
		: DynamicVertexBuffer(flags, size, stride)
	{
		RT_Resize(m_Size);
	}

	DynamicVertexBufferD3D11::~DynamicVertexBufferD3D11()
	{
		if (m_Buffer)
		{
			if (TRenderDeviceD3D11* pDevice = TRenderDeviceD3D11::Get())
			{
				pDevice->RegisterPendingDeleteResource(m_Buffer);
			}
		}
	}

	void DynamicVertexBufferD3D11::RT_Resize(usize newSize)
	{
		CHECK(IsRenderThread());
		m_Size = newSize;

		if (m_Buffer)
		{
			if (TRenderDeviceD3D11* pDevice = TRenderDeviceD3D11::Get())
			{
				pDevice->RegisterPendingDeleteResource(m_Buffer);
				m_Buffer = nullptr;
			}
		}

		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = m_Size;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		Desc.CPUAccessFlags = 0;

		if (m_AccessFlags & EBufferAccessFlags::ReadAccess)
		{
			Desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		}

		if (m_AccessFlags & EBufferAccessFlags::WriteAccess)
		{
			Desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}

		Desc.StructureByteStride = (UINT)m_Stride;

		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		HRCHECK(Device->CreateBuffer(&Desc, nullptr, &m_Buffer));
	}

}