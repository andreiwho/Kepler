#include "RenderGlobals.h"

namespace Kepler
{
	ERenderAPI GRenderAPI = ERenderAPI::Default;
	std::thread::id GRenderThreadID{};

	extern bool IsRenderThread()
	{
		return std::this_thread::get_id() == GRenderThreadID;
	}

}