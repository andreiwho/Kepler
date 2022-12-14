#include "ParamBufferD3D11.h"
#include "../RenderGlobals.h"
#include "RenderDeviceD3D11.h"
#include "../Elements/CommandList.h"
#include "CommandListImmediateD3D11.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogParamBuffer, Info);

	TParamBufferD3D11::TParamBufferD3D11(RefPtr<PipelineParamMapping> Mapping)
		:	IParamBuffer(Mapping)
	{
		CHECK(m_pParams);
		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = m_pParams->GetDataSize();
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		static constexpr usize CONSTANT_BUFFER_MULTIPLE = 16;
		if (Desc.ByteWidth > CONSTANT_BUFFER_MULTIPLE)
		{
			if (Desc.ByteWidth % CONSTANT_BUFFER_MULTIPLE > 0)
			{
				Desc.ByteWidth += CONSTANT_BUFFER_MULTIPLE - (Desc.ByteWidth % CONSTANT_BUFFER_MULTIPLE);
			}
		}
		else
		{
			Desc.ByteWidth = CONSTANT_BUFFER_MULTIPLE;
		}

		Desc.StructureByteStride = m_pParams->GetDataSize();

		D3D11_SUBRESOURCE_DATA BufferData{};
		ZeroMemory(&BufferData, sizeof(BufferData));
		BufferData.pSysMem = m_pParams->GetDataPointer();

		for (u8 idx = 0; idx < Buffer.size(); ++idx)
		{
			HRCHECK(Device->CreateBuffer(&Desc, &BufferData, &Buffer[idx]));
		}

		KEPLER_TRACE(LogParamBuffer, "Created Param Buffer with size {}", m_pParams->GetDataSize());
	}

	TParamBufferD3D11::~TParamBufferD3D11()
	{
		for (auto pBuffer : Buffer)
		{
			if (pBuffer)
			{
				auto Device = CHECKED(TRenderDeviceD3D11::Get());
				Device->RegisterPendingDeleteResource(pBuffer);
			}
		}
	}

	void TParamBufferD3D11::RT_UploadToGPU(RefPtr<ICommandListImmediate> pImmContext)
	{
		CHECK(IsRenderThread());

		if (IsRenderStateDirty())
		{
			auto MyCmd = RefCast<GraphicsCommandListImmediateD3D11>(pImmContext);
			if (MyCmd)
			{
				void* Memory = MyCmd->MapParamBuffer_NextFrame(RefFromThis<TParamBufferD3D11>());
				if (Memory)
				{
					memcpy(Memory, m_pParams->GetDataPointer(), m_pParams->GetDataSize());
					MyCmd->UnmapParamBuffer_NextFrame(RefFromThis<TParamBufferD3D11>());
				}
			}
			ResetRenderState();
		}
	}

}