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
#endif

#include "Core/Malloc.h"
#include "IndexBufferD3D11.h"
#include "ParamBufferD3D11.h"
#include "ImageD3D11.h"
#include "TransferBufferD3D11.h"
#include "RenderTargetD3D11.h"
#include "TextureD3D11.h"

namespace ke
{
	TRenderDeviceD3D11* TRenderDeviceD3D11::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
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

		m_ImmediateCommandList = MakeRef(New<GraphicsCommandListImmediateD3D11>(ImmediateContext));
	}

	//////////////////////////////////////////////////////////////////////////
	TRenderDeviceD3D11::~TRenderDeviceD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
		RT_FlushPendingDeleteResources();

		if (ClassLinkage)
			ClassLinkage->Release();
		if (ImmediateContext)
			ImmediateContext->Release();
		if (Device)
			Device->Release();
		if (Factory)
			Factory->Release();
#ifdef ENABLE_DEBUG
		if (InfoQueue)
		{
			IDXGIDebug* Debug = nullptr;
#ifdef FORCE_REPORT_LIVE_OBJECTS
			HRCHECK_NOTHROW(InfoQueue->QueryInterface(&Debug));
			HRCHECK_NOTHROW(Debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL));
#endif
			SAFE_RELEASE(Debug);
			SAFE_RELEASE(InfoQueue);
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IVertexBuffer> TRenderDeviceD3D11::CreateVertexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<IAsyncDataBlob> Data)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TVertexBufferD3D11>(InAccessFlags, Data));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IVertexBufferDynamic> TRenderDeviceD3D11::CreateDynamicVertexBuffer(EBufferAccessFlags access, usize size, usize stride)
	{
		CHECK(IsRenderThread());
		std::lock_guard	lck{ ResourceMutex };
		return MakeRef(New<DynamicVertexBufferD3D11>(access, size, stride));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IIndexBuffer> TRenderDeviceD3D11::CreateIndexBuffer(EBufferAccessFlags InAccessFlags, RefPtr<IAsyncDataBlob> Data)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TIndexBufferD3D11>(InAccessFlags, Data));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IIndexBufferDynamic> TRenderDeviceD3D11::CreateDynamicIndexBuffer(EBufferAccessFlags access, usize size, usize stride)
	{
		CHECK(IsRenderThread());
		std::lock_guard	lck{ ResourceMutex };
		return MakeRef(New<DynamicIndexBufferD3D11>(access, size, stride));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IParamBuffer> TRenderDeviceD3D11::CreateParamBuffer(RefPtr<PipelineParamMapping> Params)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TParamBufferD3D11>(Params));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ISwapChain> TRenderDeviceD3D11::CreateSwapChainForWindow(class TWindow* Window)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TSwapChainD3D11>(Window));
	}

	//////////////////////////////////////////////////////////////////////////
	void TRenderDeviceD3D11::Internal_InitInfoMessageStartIndex_Debug()
	{
#ifdef ENABLE_DEBUG
		if (InfoQueue)
		{
			InfoMsgStartIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	Array<String> TRenderDeviceD3D11::GetInfoQueueMessages() const
	{
#ifdef ENABLE_DEBUG
		Array<String> OutMessages;
		const u64 InfoMsgEndIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		for (u64 idx = InfoMsgStartIndex; idx < InfoMsgEndIndex; ++idx)
		{
			SIZE_T MessageLength{};
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, idx, nullptr, &MessageLength));
			DXGI_INFO_QUEUE_MESSAGE* pMessage = (DXGI_INFO_QUEUE_MESSAGE*)TMalloc::Get()->Allocate(MessageLength);
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, idx, pMessage, &MessageLength));
			OutMessages.EmplaceBack(pMessage->pDescription);
			TMalloc::Get()->Free(pMessage);
		}
		return OutMessages;
#else
		return {};
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	bool TRenderDeviceD3D11::RT_FlushPendingDeleteResources()
	{
		CHECK(IsRenderThread());

		bool bHasDeletedAny = false;
		ID3D11DeviceChild* pResource;
		if (PendingDeleteResources.GetLength() > 0)
		{
#ifdef ENABLE_DEBUG
			u32 NumDeletedResources{ (u32)PendingDeleteResources.GetLength() };
#endif
			std::lock_guard lck{ ResourceMutex };
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

	//////////////////////////////////////////////////////////////////////////
	void TRenderDeviceD3D11::RegisterPendingDeleteResource(ID3D11DeviceChild* Resource)
	{
		PendingDeleteResources.Enqueue(std::move(Resource));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ITextureSampler2D> TRenderDeviceD3D11::CreateTextureSampler2D(RefPtr<IImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		CHECK(IsRenderThread());
		return MakeRef(New<TTextureSampler2D_D3D11>(InImage, MipLevel, ArrayLayer));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ITransferBuffer> TRenderDeviceD3D11::CreateTransferBuffer(usize Size, RefPtr<IAsyncDataBlob> InitialData)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TTransferBufferD3D11>(Size, InitialData));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IImage2D> TRenderDeviceD3D11::CreateImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 MipLevels, u32 InArraySize)
	{
		CHECK(IsRenderThread());
		std::lock_guard lck{ ResourceMutex };
		return MakeRef(New<TImage2D_D3D11>(InWidth, InHeight, InFormat, InUsage, MipLevels, InArraySize));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IRenderTarget2D> TRenderDeviceD3D11::CreateRenderTarget2D(RefPtr<IImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return MakeRef(New<RenderTarget2D_D3D11>(InImage, MipLevel, ArrayLayer));
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IDepthStencilTarget2D> TRenderDeviceD3D11::CreateDepthStencilTarget2D(RefPtr<IImage2D> InImage, u32 MipLevel, u32 ArrayLayer, bool bReadOnly)
	{
		return MakeRef(New<DepthStencilTarget2D_D3D11>(InImage, MipLevel, ArrayLayer, bReadOnly));
	}

	//////////////////////////////////////////////////////////////////////////
	static String GetAdapterName(IDXGIAdapter* Adapter)
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

	//////////////////////////////////////////////////////////////////////////
	void TRenderDeviceD3D11::CreateFactory()
	{
		CHECK(IsRenderThread());
		UINT Flags = 0;
#ifdef ENABLE_DEBUG
		Flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRCHECK(::CreateDXGIFactory2(Flags, IID_PPV_ARGS(&Factory)));
	}

	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	void TRenderDeviceD3D11::CreateClassLinkage()
	{
		CHECK(IsRenderThread());
		CHECK(Device);

		HRCHECK(Device->CreateClassLinkage(&ClassLinkage));
	}

	//////////////////////////////////////////////////////////////////////////
	AsyncDataBlobD3D11::AsyncDataBlobD3D11(const void* Data, usize Size, usize ElemSize)
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

	AsyncDataBlobD3D11::~AsyncDataBlobD3D11()
	{
		Blob->Release();
	}

	//////////////////////////////////////////////////////////////////////////
	const void* AsyncDataBlobD3D11::GetData() const
	{
		CHECK(Blob);
		return Blob->GetBufferPointer();
	}

	//////////////////////////////////////////////////////////////////////////
	usize AsyncDataBlobD3D11::GetSize() const
	{
		CHECK(Blob);
		return Blob->GetBufferSize();
	}

	//////////////////////////////////////////////////////////////////////////
	void AsyncDataBlobD3D11::Write(const void* Data, usize Size)
	{
		CHECK(Blob);
		CHECK(Size <= GetSize());
		if (Data)
		{
			memcpy(Blob->GetBufferPointer(), Data, Size);
		}
	}

	//////////////////////////////////////////////////////////////////////////
}
#endif