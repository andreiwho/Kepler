#pragma once
#include "Core/Types.h"
#include "Platform/Window.h"
#include "Platform/PlatformEvent.h"
#include "Core/Malloc.h"
#include "Renderer/RenderThread.h"
#include "Renderer/LowLevelRenderer.h"

#include "Renderer/RenderDevice.h"

#include <string>
#include <memory>
#include "Containers/DynArray.h"
#include "Modules/ModuleStack.h"

namespace Kepler
{
	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(TDynArray<std::string> const& CommandLine);
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
		virtual void ChildSetupModuleStack(TModuleStack& ModuleStack) {}
		virtual void OnPlatformEvent(const TPlatformEventBase& Event) override;

	private:
		void InitApplicationModules();
		void TerminateModuleStack();

		bool OnWindowClosed(const TWindowClosedEvent& Event);
		bool OnWindowResized(const TWindowSizeEvent& Event);

	private:
		TWindow* MainWindow{};
		TSharedPtr<TLowLevelRenderer> LowLevelRenderer{};
		TModuleStack ModuleStack{};
	};

	extern TSharedPtr<TApplication> MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams);
}