#include "Launch.h"
#include "Platform/Platform.h"
#include "Core/App.h"
#include "Core/Log.h"
#include "Core/Malloc.h"
#include "Async/Async.h"
#include "Core/Filesystem/VFS.h"

#ifdef WIN32
# include <crtdbg.h>
#endif

namespace ke
{
	TCommandLineArguments ReadCommandLineArgs(i32 Argc, char** ppArgv)
	{
		TDynArray<TString> CommandLineArguments;
		for (i32 idx = 1; idx < Argc; ++idx)
		{
			if (char const* const arg = ppArgv[idx])
			{
				CommandLineArguments.EmplaceBack(ppArgv[idx]);
			}
		}
		return TCommandLineArguments(CommandLineArguments);
	}

	int Main(i32 Argc, char** ppArgv)
	{
		// Log must be the first one always (after malloc)
		TMalloc Malloc{};
		TLog GlobalLog;
		TVirtualFileSystem FileSystem;
		GLargeThreadPool = new TThreadPool(std::thread::hardware_concurrency() - 1); // We already have render thread

		try
		{
			// Platform must be initialized after the log and 
			auto platform = TPlatform::CreatePlatformInterface();

			std::shared_ptr<TApplication> AppInstance;
			{
				TApplicationLaunchParams Params{};
				Params.CommandLine = ReadCommandLineArgs(Argc, ppArgv);
				// ...
				AppInstance = MakeRuntimeApplication(Params);

				KEPLER_INFO(LogInit, "Resolved game path: {}", VFSResolvePath("Game://"));
			}

			if (AppInstance)
			{
				AppInstance->Run();
			}
		}
		catch (const ke::TException& Exception)
		{
			KEPLER_CRITICAL(LogInit, "{}", Exception.GetErrorMessage());
			TPlatform::HandleCrashReported(Exception.GetErrorMessage());
			return EXIT_FAILURE;
		}
		catch (const std::exception& Exception)
		{
			KEPLER_CRITICAL(LogInit, "Exception caught: {}", Exception.what());
			return EXIT_FAILURE;
		}

		delete GLargeThreadPool;
		return EXIT_SUCCESS;
	}
}

extern int main(int argc, char** argv)
{
#if defined(ENABLE_DEBUG) && defined(WIN32)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return ke::Main(argc, argv);
}
