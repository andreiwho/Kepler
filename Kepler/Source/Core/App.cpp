#include "App.h"

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
	}

	TApplication::~TApplication()
	{
	}

	void TApplication::Run()
	{
	}

}