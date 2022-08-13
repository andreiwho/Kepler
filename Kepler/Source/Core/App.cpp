#include "App.h"
#include "Log.h"

#include <iostream>

namespace Kepler
{
	TCommandLineArguments::TCommandLineArguments(std::vector<std::string> const& commandLine)
	{
		// Parse command line args
		for (const std::string& arg : commandLine)
		{
			// TODO: Do some parsing
		}
	}

	TApplication::TApplication(const TApplicationLaunchParams& LaunchParams)
	{
		TLog::Info("LogApp", "Starting application initialization");
	}

	TApplication::~TApplication()
	{
		TLog::Info("LogApp", "Finishing application termination");
	}

	void TApplication::Run()
	{
		TLog::Info("LogApp", "Application Run called...");
	}

}