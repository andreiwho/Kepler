#include "SwapChain.h"
#include "Renderer/RenderGlobals.h"
#include "Platform/Window.h"
#include "../RenderThread.h"

namespace Kepler
{

	TSwapChain::TSwapChain(class TWindow* Window)
		: MyWindow(Window)
		, Width(Window->GetWidth())
		, Height(Window->GetHeight())
		, ImageCount(3)
	{
	}

	TSwapChain::~TSwapChain()
	{
		ENQUEUE_RENDER_TASK_FLUSH([this] { ReleaseResources(); });
	}

	void TSwapChain::RT_Present()
	{
		CHECK(IsRenderThread());
		Present();
	}

	void TSwapChain::RT_Resize(i32 Width, i32 Height)
	{
		CHECK(IsRenderThread());
		Resize(Width, Height);
	}

}