#ifdef WIN32
#include "RenderDeviceD3D11.h"
#include "Core/Log.h"
#include "../RenderThread.h"

namespace Kepler
{
	TRenderDeviceD3D11::TRenderDeviceD3D11()
	{
		ENQUEUE_RENDER_TASK([this]
			{
				CreateFactory();
				CreateDevice();
			});
	}

	TRenderDeviceD3D11::~TRenderDeviceD3D11()
	{
		ENQUEUE_RENDER_TASK_FLUSH([this]
			{
				if (ImmediateContext)
					ImmediateContext->Release();
				if (Device)
					Device->Release();
				if (Factory)
					Factory->Release();
			});
	}

	static std::string GetAdapterName(IDXGIAdapter* Adapter)
	{
		if (!Adapter)
		{
			return "No Adapter";
		}

		DXGI_ADAPTER_DESC Desc;
		HRCHECK(Adapter->GetDesc(&Desc));
		return ConvertToAnsiString(Desc.Description);
	}

	void TRenderDeviceD3D11::CreateFactory()
	{
		UINT Flags = 0;
#ifndef NDEBUG
		Flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRCHECK(::CreateDXGIFactory2(Flags, IID_PPV_ARGS(&Factory)));
}

	void TRenderDeviceD3D11::CreateDevice()
	{
		CHECK(Factory);

		IDXGIAdapter* Adapter;
		HRCHECK(Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter)));

		UINT Flags = 0;
#ifndef NDEBUG
		Flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL PreferredFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		ID3D11Device* OutDevice;
		ID3D11DeviceContext* OutContext;
		D3D_FEATURE_LEVEL OutFeatureLevel;
		HRCHECK(::D3D11CreateDevice(Adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			Flags,
			PreferredFeatureLevels,
			ARRAYSIZE(PreferredFeatureLevels),
			D3D11_SDK_VERSION,
			&OutDevice, &OutFeatureLevel, &OutContext));
		CHECK(OutDevice);
		HRCHECK(OutDevice->QueryInterface(&Device));
		OutDevice->Release();

		CHECK(OutContext);
		HRCHECK(OutContext->QueryInterface(&ImmediateContext));
		OutContext->Release();
		KEPLER_TRACE("LogRender", "Created D3D11 Render Device from adapter '{}'", GetAdapterName(Adapter));
		Adapter->Release();
	}

}
#endif