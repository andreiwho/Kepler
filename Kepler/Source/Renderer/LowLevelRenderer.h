#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"

#include <vector>

namespace Kepler
{
	class TLowLevelRenderer : public TRefCounted
	{
	public:
		TLowLevelRenderer();
		~TLowLevelRenderer();

		void InitRenderStateForWindow(class TWindow* InWindow);
		void PresentAll();
		void DestroyRenderStateForWindow(class TWindow* InWindow);
		void OnWindowResized(class TWindow* InWindow);

	public:
		inline TRef<TRenderDevice> GetRenderDevice() const { return RenderDevice; }
		inline TRef<TSwapChain> GetSwapChain(u32 Index) const 
		{ 
			if (SwapChains.size() > Index)
			{
				return SwapChains.at(Index);
			}
			return nullptr;
		}

	private:
		TRef<TSwapChain> FindAssociatedSwapChain(class TWindow* InWindow) const;

	private:
		TRenderThread RenderThread{};

		TRef<TRenderDevice> RenderDevice{};
		std::vector<TRef<TSwapChain>> SwapChains;
	};
}