#pragma once
#include "Core/Types.h"
#include "RenderTypes.h"
#include "Core/Malloc.h"

#include "Elements/SwapChain.h"
#include "Elements/CommandList.h"

#include <memory>

namespace Kepler
{
	class TRenderDevice : public TRefCounted
	{
	public:
		virtual ~TRenderDevice() = default;

		static TRef<TRenderDevice> CreateRenderDevice(ERenderAPI OverrideAPI = ERenderAPI::Default);
		virtual TRef<TSwapChain> CreateSwapChainForWindow(class TWindow* Window) = 0;

		inline TRef<TCommandListImmediate> GetImmediateCommandList() const { return ImmediateCommandList; }

	protected:
		TRef<TCommandListImmediate> ImmediateCommandList{};
	};
}