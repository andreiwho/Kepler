#include "LowLevelRenderer.h"
#include "Platform/Window.h"

namespace Kepler
{
	
	TLowLevelRenderer::TLowLevelRenderer()
	{
		ENQUEUE_RENDER_TASK([this]
			{
				RenderDevice = TRenderDevice::CreateRenderDevice();
			});
	}

	TLowLevelRenderer::~TLowLevelRenderer()
	{
		ENQUEUE_RENDER_TASK_AWAITED(
			[this] 
			{
				for (auto& SwapChain : SwapChains)
				{
					SwapChain.reset();
				}
				RenderDevice.reset();
			});
	}

	void TLowLevelRenderer::InitRenderStateForWindow(class TWindow* InWindow)
	{
		ENQUEUE_RENDER_TASK_AWAITED((
			[this, InWindow]()
			{ 
				SwapChains.emplace_back(RenderDevice->CreateSwapChainForWindow(InWindow)); 
			})
		);
	}

	void TLowLevelRenderer::PresentAll()
	{
		ENQUEUE_RENDER_TASK_AWAITED([this] 
			{
				for (const auto& SwapChain : SwapChains)
				{
					SwapChain->Present();
				}
			});
	}

	void TLowLevelRenderer::DestroyRenderStateForWindow(class TWindow* InWindow)
	{
		ENQUEUE_RENDER_TASK_AWAITED(([this, InWindow]
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
			}));
	}

	void TLowLevelRenderer::OnWindowResized(class TWindow* InWindow)
	{
		ENQUEUE_RENDER_TASK_AWAITED(([this, InWindow]
			{
				if (auto SwapChain = FindAssociatedSwapChain(InWindow))
				{
					SwapChain->Resize(InWindow->GetWidth(), InWindow->GetHeight());
				}
			}));
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