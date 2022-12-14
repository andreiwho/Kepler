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
	TCommandLineArguments ReadCommandLineArgs(i32 argc, char** ppArgv)
	{
		Array<String> cmdArgs;
		for (i32 idx = 1; idx < argc; ++idx)
		{
			if (char const* const arg = ppArgv[idx])
			{
				cmdArgs.EmplaceBack(ppArgv[idx]);
			}
		}
		return TCommandLineArguments(cmdArgs);
	}

	int Main(i32 argc, char** ppArgv)
	{
		// Log must be the first one always (after malloc)
		TMalloc poolAlloc{};
		TLog globalLog;
		TVirtualFileSystem VFS;
		GLargeThreadPool = new TThreadPool(std::thread::hardware_concurrency() - 1); // We already have render thread

		try
		{
			// Platform must be initialized after the log and 
			auto pPlatform = TPlatform::CreatePlatformInterface();

			std::shared_ptr<Engine> pApp;
			{
				TApplicationLaunchParams params{};
				params.CommandLine = ReadCommandLineArgs(argc, ppArgv);
				// ...
				pApp = MakeRuntimeApplication(params);

				KEPLER_INFO(LogInit, "Resolved game path: {}", VFSResolvePath("Game://"));
			}

			if (pApp)
			{
				pApp->Run();
			}
		}
		catch (const ke::TException& exc)
		{
			KEPLER_CRITICAL(LogInit, "{}", exc.GetErrorMessage());
			TPlatform::HandleCrashReported(exc.GetErrorMessage());
			return EXIT_FAILURE;
		}
		catch (const std::exception& exc)
		{
			KEPLER_CRITICAL(LogInit, "Exception caught: {}", exc.what());
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

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <string>
#include <filesystem>
static ke::String GetExecutablePath()
{
	CHAR exePath[1024];
	DWORD len = GetModuleFileNameA(NULL, exePath, ARRAYSIZE(exePath));
	if (len < 1024)
	{
		exePath[len + 1] = 0;
	}
	return exePath;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	using namespace ke;

	auto wideString = ConvertToWideString(lpCmdLine);

	INT numArgs = 0;
	LPWSTR* pWCmdLine = CommandLineToArgvW(wideString.c_str(), &numArgs);
	std::vector<std::string> cxxConvertedParams((usize)numArgs);
	for (INT index = 0; index < numArgs; ++index)
	{
		cxxConvertedParams[index] = ConvertToAnsiString(pWCmdLine[index]);
	}
	
	String exePath = GetExecutablePath();
	std::vector<char*> cArgv;
	cArgv.reserve((usize)numArgs + 1);
	cArgv.emplace_back(exePath.data());

	for (auto& param : cxxConvertedParams)
	{
		cArgv.emplace_back(param.data());
	}
	return main(numArgs + 1, cArgv.data());
}
#endif
