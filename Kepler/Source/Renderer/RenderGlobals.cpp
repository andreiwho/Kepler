#include "RenderGlobals.h"
#include "LowLevelRenderer.h"

namespace Kepler
{
	ERenderAPI GRenderAPI = ERenderAPI::Default;
	std::thread::id GRenderThreadID{};

	extern bool IsRenderThread()
	{
		return std::this_thread::get_id() == GRenderThreadID;
	}

	class TRef<TRenderDevice> GetRenderDevice()
	{
		return TLowLevelRenderer::Get()->GetRenderDevice();
	}
}