#ifdef WIN32
#include "SwapChainD3D11.h"
#include "D3D11Common.h"
#include "RenderDeviceD3D11.h"
#include "Platform/Window.h"
#include "Renderer/RenderGlobals.h"

#include <sstream>

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogSwapchain, All);

	TSwapChainD3D11::TSwapChainD3D11(class TWindow* Window)
		: TSwapChain(Window)
	{
		CHECK(IsRenderThread());
		DXGI_SWAP_CHAIN_DESC1 Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Width = Width;
		Desc.Height = Height;
		Desc.SampleDesc = { 1, 0 };
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.Stereo = false;
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = ImageCount;
		Desc.Scaling = DXGI_SCALING_STRETCH;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		Desc.Flags = 0;

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK(Device);

		IDXGISwapChain1* NewChain;
		HRCHECK(Device->GetFactory()->CreateSwapChainForHwnd(
			Device->GetDevice(),
			(HWND)Window->GetNativeHandle(),
			&Desc,
			nullptr,
			nullptr,
			&NewChain
		));
		HRCHECK(NewChain->QueryInterface(&SwapChain));
		NewChain->Release();

		CreateRenderTargets();
	}

	TSwapChainD3D11::~TSwapChainD3D11()
	{
		if (RenderTargetView)
			RenderTargetView->Release();

		if (SwapChain)
			SwapChain->Release();
	}

	void TSwapChainD3D11::CreateRenderTargets()
	{
		CHECK(IsRenderThread() && SwapChain);

		if (RenderTargetView)
		{
			RenderTargetView->Release();
		}

		CComPtr<ID3D11Resource> Buffer{};
		HRCHECK(SwapChain->GetBuffer(0, IID_PPV_ARGS(&Buffer)));

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK(Device);

		HRCHECK(Device->GetDevice()->CreateRenderTargetView1(Buffer, nullptr, &RenderTargetView));
	}

	void TSwapChainD3D11::Present()
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		CHECK(SwapChain);
		
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK(Device);
		Device->Internal_InitInfoMessageStartIndex_Debug();
		const HRESULT Result = SwapChain->Present(1, 0);
		if (FAILED(Result))
		{
			auto Messages = Device->GetInfoQueueMessages();
			if (!Messages.IsEmpty())
			{
				std::stringstream StringStream;
				for (const auto& Message : Messages)
				{
					StringStream << Message << '\n';
				}
				CHECKMSG(false, fmt::format("DXGI Error list:\n{}", StringStream.str()));
			}
		}
	}

	void TSwapChainD3D11::Resize(i32 InWidth, i32 InHeight)
	{
		CHECK(IsRenderThread());

		if (RenderTargetView)
		{
			RenderTargetView->Release();
			RenderTargetView = nullptr;
		}
		
		Width = InWidth > 0 ? InWidth : 1;
		Height = InHeight > 0 ? InHeight : 1;
		HRCHECK(SwapChain->ResizeBuffers(ImageCount, (UINT)Width, (UINT)Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		CreateRenderTargets();
	}
}
#endif