#include "RenderGlobals.h"
#include "LowLevelRenderer.h"

namespace ke
{
	ERenderAPI GRenderAPI = ERenderAPI::Default;
	std::thread::id GRenderThreadID{};

	extern bool IsRenderThread()
	{
		return std::this_thread::get_id() == GRenderThreadID;
	}

	class RefPtr<TRenderDevice> GetRenderDevice()
	{
		return LowLevelRenderer::Get()->GetRenderDevice();
	}
}