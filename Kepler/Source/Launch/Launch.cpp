#include "Launch.h"
#include "Core/App.h"
#include "Core/Log.h"

namespace Kepler
{
	TCommandLineArguments ReadCommandLineArgs(i32 Argc, char** ppArgv)
	{
		std::vector<std::string> CommandLineArguments;
		for (i32 Index = 1; Index < Argc; ++Index)
		{
			if (char const* const arg = ppArgv[Index])
			{
				CommandLineArguments.emplace_back(ppArgv[Index]);
			}
		}
		return TCommandLineArguments(CommandLineArguments);
	}

	int Main(i32 Argc, char** ppArgv)
	{
		TLog GlobalLog;
		std::shared_ptr<TApplication> AppInstance;
		{
			TApplicationLaunchParams Params{};
			Params.CommandLine = ReadCommandLineArgs(Argc, ppArgv);
			// ...
			AppInstance = MakeRuntimeApplication(Params);
		}

		if (AppInstance)
		{
			AppInstance->Run();
			return EXIT_SUCCESS;
		}

		return EXIT_FAILURE;
	}
}

extern int main(int argc, char** argv)
{
	return Kepler::Main(argc, argv);
}
