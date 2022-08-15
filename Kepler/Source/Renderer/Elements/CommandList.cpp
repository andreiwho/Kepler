#include "CommandList.h"
#include "Renderer/RenderThread.h"

namespace Kepler
{

	void TImmediateCommandListProxy::StartDrawingToSwapChainImage(TSwapChain* SwapChain)
	{
		ENQUEUE_RENDER_TASK(([this, SwapChain] { CommandList->StartDrawingToSwapChainImage(SwapChain); }));
	}

	void TImmediateCommandListProxy::ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4])
	{
		ENQUEUE_RENDER_TASK(([this, SwapChain, ClearColor] { CommandList->ClearSwapChainImage(SwapChain, ClearColor); }));
	}

}