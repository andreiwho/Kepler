#ifdef WIN32
#include "RenderDeviceD3D11.h"
#include "Core/Log.h"
#include "Renderer/RenderThread.h"
#include "SwapChainD3D11.h"
#include "Platform/Window.h"
#include "Core/Macros.h"
#include "Renderer/RenderGlobals.h"

#ifndef NDEBUG
# include <dxgidebug.h>
#include "Core/Malloc.h"
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
#ifndef NDEBUG
		InitializeInfoQueue();
#endif
	}

	TRenderDeviceD3D11::~TRenderDeviceD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
#ifndef NDEBUG
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

	TRef<TSwapChain> TRenderDeviceD3D11::CreateSwapChainForWindow(class TWindow* Window)
	{
		CHECK(IsRenderThread());
		return MakeRef<TSwapChainD3D11>(Window);
	}

	void TRenderDeviceD3D11::Internal_InitInfoMessageStartIndex_Debug()
	{
#ifndef NDEBUG
		if (InfoQueue)
		{
			InfoMsgStartIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		}
#endif
	}

	std::vector<std::string> TRenderDeviceD3D11::GetInfoQueueMessages() const
	{
#ifndef NDEBUG
		std::vector<std::string> OutMessages;
		const u64 InfoMsgEndIndex = InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		for (u64 Index = InfoMsgStartIndex; Index < InfoMsgEndIndex; ++Index)
		{
			SIZE_T MessageLength{};
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, Index, nullptr, &MessageLength));
			DXGI_INFO_QUEUE_MESSAGE* pMessage = (DXGI_INFO_QUEUE_MESSAGE*)TMalloc::Get()->Allocate(MessageLength);
			HRCHECK(InfoQueue->GetMessageA(DXGI_DEBUG_ALL, Index, pMessage, &MessageLength));
			OutMessages.emplace_back(pMessage->pDescription);
			TMalloc::Get()->Free(pMessage);
		}
		return OutMessages;
#else
		return {};
#endif
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
#ifndef NDEBUG
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
#ifndef NDEBUG
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
		KEPLER_TRACE("LogRender", "Created D3D11 Render Device from adapter '{}'", GetAdapterName(Adapter));
		Adapter->Release();
	}

	void TRenderDeviceD3D11::InitializeInfoQueue()
	{
#ifndef NDEBUG
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

}
#endif