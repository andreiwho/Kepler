#include "RenderDevice.h"
#include "RenderGlobals.h"
#include "Core/Log.h"

#ifdef WIN32
# include "RendererD3D11/RenderDeviceD3D11.h"
#endif

namespace Kepler
{

	TRef<Kepler::TRenderDevice> TRenderDevice::CreateRenderDevice(ERenderAPI OverrideAPI)
	{
		switch (OverrideAPI)
		{
		case Kepler::ERenderAPI::Default:
#ifdef WIN32
			return CreateRenderDevice(ERenderAPI::DirectX11);
#endif
			break;
#ifdef WIN32
		case Kepler::ERenderAPI::DirectX11:
			KEPLER_TRACE("LogRender", "Creating RenderDevice for DirectX11 API");
			GRenderAPI = ERenderAPI::DirectX11;
			return AsRef<TRenderDevice>(New<TRenderDeviceD3D11>());
			break;
#endif
		default:
			break;
		}
		CHECK(false && "Failed to create render device. Unknown render API");
	}

}