#include "RenderDevice.h"
#include "RenderGlobals.h"
#include "Core/Log.h"

#ifdef WIN32
# include "RendererD3D11/RenderDeviceD3D11.h"
#endif

namespace ke
{
	RefPtr<ke::TRenderDevice> TRenderDevice::CreateRenderDevice(ERenderAPI overrideApi)
	{
		switch (overrideApi)
		{
		case ke::ERenderAPI::Default:
#ifdef WIN32
			return CreateRenderDevice(ERenderAPI::DirectX11);
#endif
			break;
#ifdef WIN32
		case ke::ERenderAPI::DirectX11:
			KEPLER_TRACE(LogRender, "Creating RenderDevice for DirectX11 API");
			GRenderAPI = ERenderAPI::DirectX11;
			return MakeRef(New<TRenderDeviceD3D11>());
			break;
#endif
		default:
			break;
		}
		CRASHMSG("Failed to create render device. Unknown render API");
	}

	RefPtr<AsyncDataBlob> AsyncDataBlob::CreateGraphicsDataBlob(const void* pData, usize size, usize elemSize)
	{
		switch (GRenderAPI)
		{
#ifdef WIN32
		case ke::ERenderAPI::DirectX11:
			return MakeRef(ke::New<AsyncDataBlobD3D11>(pData, size, elemSize));
#endif
		default:
			break;
		}
		CRASHMSG("Failed to create data blob. Unknown render API");
		return nullptr;
	}
}