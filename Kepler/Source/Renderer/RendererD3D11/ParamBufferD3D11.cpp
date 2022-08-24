#include "ParamBufferD3D11.h"
#include "../RenderGlobals.h"
#include "RenderDeviceD3D11.h"
#include "../Elements/CommandList.h"
#include "CommandListImmediateD3D11.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogParamBuffer);

	TParamBufferD3D11::TParamBufferD3D11(TRef<TPipelineParamPack> Params)
		:	TParamBuffer(Params)
	{
		CHECK(Params);
		CHECK(Params->IsCompiled());
		auto Device = CHECKED(TRenderDeviceD3D11::Get()->GetDevice());
		D3D11_BUFFER_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.ByteWidth = Params->GetDataSize();
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

		Desc.StructureByteStride = Params->GetDataSize();

		D3D11_SUBRESOURCE_DATA BufferData{};
		ZeroMemory(&BufferData, sizeof(BufferData));
		BufferData.pSysMem = Params->GetDataPointer();

		HRCHECK(Device->CreateBuffer(&Desc, &BufferData, &Buffer));

		KEPLER_TRACE(LogParamBuffer, "Created Param Buffer with size {}", Params->GetDataSize());
	}

	TParamBufferD3D11::~TParamBufferD3D11()
	{
		if (Buffer)
		{
			auto Device = CHECKED(TRenderDeviceD3D11::Get());
			Device->RegisterPendingDeleteResource(Buffer);
		}
	}

	void TParamBufferD3D11::RT_UploadToGPU(TRef<TCommandListImmediate> pImmContext)
	{
		CHECK(IsRenderThread());
		auto MyCmd = RefCast<TCommandListImmediateD3D11>(pImmContext);
		if (MyCmd)
		{
			void* Memory = MyCmd->MapBuffer(RefFromThis());
			if (Memory)
			{
				memcpy(Memory, Params->GetDataPointer(), Params->GetDataSize());
				MyCmd->UnmapBuffer(RefFromThis());
			}
		}
	}

}