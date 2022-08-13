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
	}

	TApplication::~TApplication()
	{
		KEPLER_INFO("LogApp", "Finishing application termination");
	}

	void TApplication::Run()
	{
		KEPLER_INFO("LogApp", "Application Run called...");

		if (TPlatform* Platform = TPlatform::Get())
		{
			while (Platform->HasActiveMainWindow())
			{
				Platform->Update();
			}
		}
	}

	void TApplication::OnPlatformEvent(const TPlatformEventBase& Event)
	{
	}

}