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
		: ISwapChain(Window)
	{
		CHECK(IsRenderThread());
		DXGI_SWAP_CHAIN_DESC1 Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Width = m_Width;
		Desc.Height = m_Height;
		Desc.SampleDesc = { 1, 0 };
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.Stereo = false;
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = m_ImageCount;
		Desc.Scaling = DXGI_SCALING_STRETCH;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

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
		SAFE_RELEASE(NewChain);

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

		ID3D11Resource* Buffer{};
		HRCHECK(SwapChain->GetBuffer(0, IID_PPV_ARGS(&Buffer)));

		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK(Device);

		HRCHECK(Device->GetDevice()->CreateRenderTargetView1(Buffer, nullptr, &RenderTargetView));
		SAFE_RELEASE(Buffer);
	}

	void TSwapChainD3D11::Present(bool bWaitForVerticalBlank)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		CHECK(SwapChain);
		
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK(Device);
		Device->Internal_InitInfoMessageStartIndex_Debug();
		const HRESULT Result = SwapChain->Present(bWaitForVerticalBlank ? 1 : 0, bWaitForVerticalBlank ? 0 : DXGI_PRESENT_ALLOW_TEARING);
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
		
		m_Width = InWidth > 0 ? InWidth : 1;
		m_Height = InHeight > 0 ? InHeight : 1;
		HRCHECK(SwapChain->ResizeBuffers(m_ImageCount, (UINT)m_Width, (UINT)m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));
		CreateRenderTargets();
	}
}
#endif