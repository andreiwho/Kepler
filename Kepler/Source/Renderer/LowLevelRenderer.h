#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"

#include <vector>

namespace Kepler
{
	class TLowLevelRenderer
	{
	public:
		TLowLevelRenderer();
		~TLowLevelRenderer();

		void InitRenderStateForWindow(class TWindow* InWindow);
		void PresentAll();
		void DestroyRenderStateForWindow(class TWindow* InWindow);
		void OnWindowResized(class TWindow* InWindow);

	private:
		TRef<TSwapChain> FindAssociatedSwapChain(class TWindow* InWindow) const;

	private:
		TRenderThread RenderThread{};

		TRef<TRenderDevice> RenderDevice{};
		std::vector<TRef<TSwapChain>> SwapChains;
	};
}