#pragma once
#include "Core/Types.h"
#include "RenderTypes.h"
#include "RenderThread.h"

#include <thread>

namespace Kepler
{
	extern ERenderAPI GRenderAPI;
	extern std::thread::id GRenderThreadID;
	
	extern bool IsRenderThread();
}