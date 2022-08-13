#pragma once
#include "Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace Kepler
{
	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(std::vector<std::string> const& commandLine);
	};

	struct TApplicationLaunchParams
	{
		TCommandLineArguments CommandLine;
	};

	// --------------------------------------------
	// This is an important class.
	// - All of the internal initialization and application logic will happen inside the IApplication::Run function
	// --------------------------------------------
	class TApplication
	{
	public:
		TApplication(const TApplicationLaunchParams& LaunchParams);
		virtual ~TApplication();

		virtual void Run();
	};

	extern std::shared_ptr<TApplication> MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams);
}