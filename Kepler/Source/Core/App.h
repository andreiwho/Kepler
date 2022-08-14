#pragma once
#include "Core/Types.h"
#include "Platform/Window.h"
#include "Platform/PlatformEvent.h"
#include "Core/Malloc.h"
#include "Renderer/RenderThread.h"

#include "Renderer/RenderDevice.h"

#include <string>
#include <vector>
#include <memory>

namespace Kepler
{
	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(std::vector<std::string> const& CommandLine);
	};

	struct TApplicationLaunchParams
	{
		TCommandLineArguments CommandLine;
	};

	// --------------------------------------------
	// This is an important class.
	// - All of the internal initialization and application logic will happen inside the IApplication::Run function
	// --------------------------------------------
	class TApplication : public IPlatformEventListener
	{
	public:
		TApplication(const TApplicationLaunchParams& LaunchParams);
		virtual ~TApplication();

		virtual void Run();
		
	protected:
		virtual void OnPlatformEvent(const TPlatformEventBase& Event) override;

	private:
		TWindow* MainWindow{};
		TRenderThread RenderThread{};
		
		TRef<TRenderDevice> RenderDevice{};
		TRef<TSwapChain> SwapChain{};
	};

	extern TRef<TApplication> MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams);
}