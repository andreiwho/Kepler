#include "SwapChain.h"
#include "Renderer/RenderGlobals.h"
#include "Platform/Window.h"
#include "../RenderThread.h"
#include "Core/Log.h"

namespace ke
{

	TSwapChain::TSwapChain(class TWindow* Window)
		: MyWindow(Window)
		, Width(Window->GetWidth())
		, Height(Window->GetHeight())
		, ImageCount(3)
	{
		CHECK(IsRenderThread());
	}
}