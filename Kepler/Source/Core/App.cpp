#include "App.h"
#include "Log.h"

#include "Platform/Platform.h"

#include <iostream>

namespace Kepler
{
	TCommandLineArguments::TCommandLineArguments(std::vector<std::string> const& CommandLine)
	{
		// Parse command line args
		for (const std::string& arg : CommandLine)
		{
			// TODO: Do some parsing
		}
	}

	TApplication::TApplication(const TApplicationLaunchParams& LaunchParams)
	{
		KEPLER_INFO("LogApp", "Starting application initialization");

		MainWindow = CHECKED(CHECKED(TPlatform::Get())->CreatePlatformWindow(1280, 720, "Kepler"));
		RenderDevice = CHECKED(TRenderDevice::CreateRenderDevice(ERenderAPI::Default));
		ENQUEUE_RENDER_TASK([this] { this->SwapChain = RenderDevice->CreateSwapChainForWindow(MainWindow); });
	}

	TApplication::~TApplication()
	{
		RenderDevice.reset();
		KEPLER_INFO("LogApp", "Finishing application termination");
	}

	void TApplication::Run()
	{
		KEPLER_INFO("LogApp", "Application Run called...");

		// Begin main loop
		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			while (Platform->HasActiveMainWindow())
			{
				Platform->Update();

				ENQUEUE_RENDER_TASK([this] { SwapChain->RT_Present(); });
				RenderThread.FlushAndWait();
			}
		}
	}

	void TApplication::OnPlatformEvent(const TPlatformEventBase& Event)
	{
	}

}