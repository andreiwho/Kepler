#ifdef WIN32
#include "RenderDeviceD3D11.h"
#include "Core/Log.h"
#include "Renderer/RenderThread.h"
#include "SwapChainD3D11.h"
#include "Platform/Window.h"
#include "Core/Macros.h"
#include "Renderer/RenderGlobals.h"
#include "CommandListImmediateD3D11.h"
#include "VertexBufferD3D11.h"

#ifdef ENABLE_DEBUG
# include <dxgidebug.h>
#include "Core/Malloc.h"
#include "IndexBufferD3D11.h"
#endif

namespace Kepler
{
	TRenderDeviceD3D11* TRenderDeviceD3D11::Instance = nullptr;

	TRenderDeviceD3D11::TRenderDeviceD3D11()
	{
		CHECK(!Instance);
		CHECK(IsRenderThread());

		Instance = this;

		CreateFactory();
		CreateDevice();
#ifdef ENABLE_DEBUG
		InitializeInfoQueue();
#endif
		CreateClassLinkage();

		ImmediateCommandList = MakeRef(New<TCommandListImmediateD3D11>(ImmediateContext));
	}

	TRenderDeviceD3D11::~TRenderDeviceD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
		RT_FlushPendingDeleteResources();

		if (ClassLinkage)
			ClassLinkage->Release();
#ifdef ENABLE_DEBUG
		if (InfoQueue)
			InfoQueue->Release();
#endif
		if (ImmediateContext)
			ImmediateContext->Release();
		if (Device)
			Device->Release();
		if (Factory)
			Factory->Release();
	}

	TRef<TVertexBuffer> TRenderDeviceD3D11::CreateVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
	{
		CHECK(IsRenderThread());
		std::lock_guard Lck{ ResourceMutex };
		return MakeRef(New<TVertexBufferD3D11>(InAccessFlags, Data));
	}

	TRef<TIndexBuffer> TRenderDeviceD3D11::CreateIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data)
	{
		CHECK(IsRenderThread());
		std::lock_guard Lck{ ResourceMutex };
		return MakeRef(New<TIndexBufferD3D11>(InAccessFlags, Data));
	}

	TRef<TSwapChain> TRenderDeviceD3D11::CreateSwapChainForWindow(class TWindow* Window)
	{
		CHECK(IsRenderThread());
		return MakeRef(New<TSwapChainD3D11>(Window));
	}

	void TRenderDeviceD3D11::Internal_InitInfoMessageStartIndex_Debug()
	{
#ifdef ENABLE_DEBUG
		if (InfoQueue)
		{
			InfoMsgStartIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		}
#endif
	}

	TDynArray<std::string> TRenderDeviceD3D11::GetInfoQueueMessages() const
	{
#ifdef ENABLE_DEBUG
		TDynArray<std::string> OutMessages;
		const u64 InfoMsgEndIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		for (u64 Index = InfoMsgStartIndex; Index < InfoMsgEndIndex; ++Index)
		{
			SIZE_T MessageLength{};
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, Index, nullptr, &MessageLength));
			DXGI_INFO_QUEUE_MESSAGE* pMessage = (DXGI_INFO_QUEUE_MESSAGE*)TMalloc::Get()->Allocate(MessageLength);
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, Index, pMessage, &MessageLength));
			OutMessages.EmplaceBack(pMessage->pDescription);
			TMalloc::Get()->Free(pMessage);
		}
		return OutMessages;
#else
		return {};
#endif
	}

	bool TRenderDeviceD3D11::RT_FlushPendingDeleteResources()
	{
		CHECK(IsRenderThread());

		bool bHasDeletedAny = false;
		ID3D11Resource* pResource;
		if (PendingDeleteResources.GetLength() > 0)
		{
#ifdef ENABLE_DEBUG
			u32 NumDeletedResources{ (u32)PendingDeleteResources.GetLength() };
#endif
			std::lock_guard Lck{ ResourceMutex };
			while (PendingDeleteResources.Dequeue(pResource))
			{
				pResource->Release();
				bHasDeletedAny = true;
			}
#ifdef ENABLE_DEBUG
			KEPLER_TRACE(LogRender, "RT_FlushPendingDeleteResources - destroyed {} D3D11 resources.", NumDeletedResources);
#endif
		}
		return bHasDeletedAny;
	}

	void TRenderDeviceD3D11::RegisterPendingDeleteResource(ID3D11Resource* Resource)
	{
		PendingDeleteResources.Enqueue(std::move(Resource));
	}

	static std::string GetAdapterName(IDXGIAdapter* Adapter)
	{
		CHECK(IsRenderThread());
		if (!Adapter)
		{
			return "No Adapter";
		}

		DXGI_ADAPTER_DESC Desc;
		HRCHECK(Adapter->GetDesc(&Desc));
		return ConvertToAnsiString(Desc.Description);
	}

	void TRenderDeviceD3D11::CreateFactory()
	{
		CHECK(IsRenderThread());
		UINT Flags = 0;
#ifdef ENABLE_DEBUG
		Flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRCHECK(::CreateDXGIFactory2(Flags, IID_PPV_ARGS(&Factory)));
	}

	void TRenderDeviceD3D11::CreateDevice()
	{
		CHECK(IsRenderThread());
		CHECK(Factory);

		IDXGIAdapter* Adapter;
		HRCHECK(Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter)));

		UINT Flags = 0;
#ifdef ENABLE_DEBUG
		Flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL PreferredFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		ID3D11Device* OutDevice;
		ID3D11DeviceContext* OutContext;
		D3D_FEATURE_LEVEL OutFeatureLevel;
		HRCHECK(::D3D11CreateDevice(Adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			Flags,
			PreferredFeatureLevels,
			ARRAYSIZE(PreferredFeatureLevels),
			D3D11_SDK_VERSION,
			&OutDevice, &OutFeatureLevel, &OutContext));
		CHECK(OutDevice);
		HRCHECK(OutDevice->QueryInterface(&Device));
		OutDevice->Release();

		CHECK(OutContext);
		HRCHECK(OutContext->QueryInterface(&ImmediateContext));
		OutContext->Release();
		KEPLER_TRACE(LogRender, "Created D3D11 Render Device from adapter '{}'", GetAdapterName(Adapter));
		Adapter->Release();
	}

	void TRenderDeviceD3D11::InitializeInfoQueue()
	{
#ifdef ENABLE_DEBUG
		HMODULE DxgiDebug = CHECKED(::LoadLibraryA("DXGIDebug.dll"));
		using TPFN_DXGIGetDebugInterface = HRESULT(*)(REFIID, void**);
		auto LoadFunc = (TPFN_DXGIGetDebugInterface)::GetProcAddress(DxgiDebug, "DXGIGetDebugInterface");
		CHECK(LoadFunc);
		HRCHECK(LoadFunc(IID_PPV_ARGS(&InfoQueue)));
		if (InfoQueue)
		{
			InfoMsgStartIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		}
#endif
	}

	void TRenderDeviceD3D11::CreateClassLinkage()
	{
		CHECK(IsRenderThread());
		CHECK(Device);

		HRCHECK(Device->CreateClassLinkage(&ClassLinkage));
	}

	TDataBlobD3D11::TDataBlobD3D11(const void* Data, usize Size, usize ElemSize)
		: Stride(ElemSize)
	{
		if (Size > 0)
		{
			HRCHECK(D3DCreateBlob(Size, &Blob));
			CHECK(Blob);
			if (Data)
			{
				Write(Data, Size);
			}
		}
	}

	const void* TDataBlobD3D11::GetData() const
	{
		CHECK(Blob);
		return Blob->GetBufferPointer();
	}

	usize TDataBlobD3D11::GetSize() const
	{
		CHECK(Blob);
		return Blob->GetBufferSize();
	}

	void TDataBlobD3D11::Write(const void* Data, usize Size)
	{
		CHECK(Blob);
		CHECK(Size <= GetSize());
		if (Data)
		{
			memcpy(Blob->GetBufferPointer(), Data, Size);
		}
	}
}
#endif