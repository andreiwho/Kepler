#pragma once
#include "Core/Core.h"
#include "RenderTypes.h"
#include "RenderThread.h"

DEFINE_UNIQUE_LOG_CHANNEL(LogRender);

#include <thread>

namespace Kepler
{
	class TRenderDevice;

	extern ERenderAPI GRenderAPI;
	extern std::thread::id GRenderThreadID;
	
	extern bool IsRenderThread();
	extern TRef<TRenderDevice> GetRenderDevice();
}