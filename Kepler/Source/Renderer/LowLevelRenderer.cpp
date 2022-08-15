#include "LowLevelRenderer.h"
#include "Platform/Window.h"

namespace Kepler
{
	
	TLowLevelRenderer::TLowLevelRenderer()
	{
		TRenderThread::Submit([this]
			{
				RenderDevice = TRenderDevice::CreateRenderDevice();
			});
		TRenderThread::Wait();
	}

	TLowLevelRenderer::~TLowLevelRenderer()
	{
		TRenderThread::Submit(
			[this] 
			{
				SwapChains.clear();
				RenderDevice.Release();
			});
		TRenderThread::Wait();
	}

	void TLowLevelRenderer::InitRenderStateForWindow(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]()
			{ 
				SwapChains.emplace_back(RenderDevice->CreateSwapChainForWindow(InWindow)); 
			}
		);
	}

	void TLowLevelRenderer::PresentAll()
	{
		TRenderThread::Submit(
			[this]
			{
				for (const auto& SwapChain : SwapChains)
				{
					SwapChain->Present();
				}
			});
		TRenderThread::Wait();
	}

	void TLowLevelRenderer::DestroyRenderStateForWindow(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]
			{
				auto FoundSwapChain = std::find_if(std::begin(SwapChains), std::end(SwapChains), 
					[InWindow](const auto& SwapChain) 
					{
						return SwapChain->GetAssociatedWindow() == InWindow;
					}
				);
				if (FoundSwapChain != SwapChains.end())
				{
					SwapChains.erase(FoundSwapChain);
				}
				SwapChains.shrink_to_fit();
			});
		TRenderThread::Wait();
	}

	void TLowLevelRenderer::OnWindowResized(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]
			{
				if (auto SwapChain = FindAssociatedSwapChain(InWindow))
				{
					SwapChain->Resize(InWindow->GetWidth(), InWindow->GetHeight());
				}
			});
	}

	TRef<TSwapChain> TLowLevelRenderer::FindAssociatedSwapChain(class TWindow* InWindow) const
	{
		auto FoundSwapChain = std::find_if(std::begin(SwapChains), std::end(SwapChains),
			[InWindow](const auto& SwapChain)
			{
				return SwapChain->GetAssociatedWindow() == InWindow;
			}
		);
		if (FoundSwapChain != SwapChains.end())
		{
			return *FoundSwapChain;
		}
		return nullptr;
	}

}