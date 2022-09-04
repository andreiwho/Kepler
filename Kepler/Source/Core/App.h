#pragma once
#include "Core/Types.h"
#include "Platform/Window.h"
#include "Platform/PlatformEvent.h"
#include "Core/Malloc.h"
#include "Renderer/RenderThread.h"
#include "Renderer/LowLevelRenderer.h"
#include "Tools/MaterialLoader.h"

#include "Audio/AudioEngine.h"

#include "Renderer/RenderDevice.h"

#include <string>
#include <memory>
#include "Containers/DynArray.h"
#include "Modules/ModuleStack.h"
#include "World/WorldRegistry.h"
#include "World/Game/GameWorld.h"
#include "Tools/ImageLoader.h"
#include "Tools/MeshLoader.h"

namespace Kepler
{
	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(TDynArray<TString> const& CommandLine);

		TString GameModuleDirectory = "";
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
		void InitVFSAliases(const TApplicationLaunchParams& LaunchParams);

		bool OnWindowClosed(const TWindowClosedEvent& Event);
		bool OnWindowResized(const TWindowSizeEvent& Event);
		bool OnKeyDown(const TKeyDownEvent& Event);
	private:
		TWindow* MainWindow{};
		TSharedPtr<TLowLevelRenderer> LowLevelRenderer{};
		TSharedPtr<TAudioEngine> AudioEngine{};
		TSharedPtr<TWorldRegistry> WorldRegistry{};

		TMaterialLoader MaterialLoader;
		TImageLoader ImageLoader;
		TMeshLoader MeshLoader;

		TRef<TGameWorld> CurrentWorld;

		TModuleStack ModuleStack{};
	};

	extern TSharedPtr<TApplication> MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams);
}