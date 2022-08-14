#pragma once
#include "Core/Types.h"
#include "RenderTypes.h"
#include "Core/Malloc.h"

#include "Elements/SwapChain.h"

#include <memory>

namespace Kepler
{
	class TRenderDevice
	{
	public:
		virtual ~TRenderDevice() = default;

		static TRef<TRenderDevice> CreateRenderDevice(ERenderAPI OverrideAPI = ERenderAPI::Default);

		virtual TRef<TSwapChain> CreateSwapChainForWindow(class TWindow* Window) = 0;
	};
}