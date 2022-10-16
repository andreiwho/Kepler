#include "SwapChain.h"
#include "Renderer/RenderGlobals.h"
#include "Platform/Window.h"
#include "../RenderThread.h"
#include "Core/Log.h"

namespace ke
{

	ISwapChain::ISwapChain(class TWindow* pWindow)
		: m_Window(pWindow)
		, m_Width(pWindow->GetWidth())
		, m_Height(pWindow->GetHeight())
		, m_ImageCount(3)
	{
		CHECK(IsRenderThread());
	}
}