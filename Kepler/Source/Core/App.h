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

namespace ke
{
	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(Array<TString> const& cmdLine);

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
	class Engine : public IPlatformEventListener
	{
	public:
		Engine(const TApplicationLaunchParams& launchParams);
		virtual ~Engine();

		virtual void Run();

	protected:
		virtual void ChildSetupModuleStack(TModuleStack& moduleStack) {}
		virtual void OnPlatformEvent(const TPlatformEventBase& event) override;

	private:
		void InitApplicationModules();
		void TerminateModuleStack();
		void InitVFSAliases(const TApplicationLaunchParams& launchParams);

		bool OnWindowClosed(const TWindowClosedEvent& event);
		bool OnWindowResized(const TWindowSizeEvent& event);
		bool OnKeyDown(const TKeyDownEvent& evemt);
	private:
		TWindow* m_MainWindow{};
		TSharedPtr<TLowLevelRenderer> m_LowLevelRenderer{};
		TSharedPtr<AudioEngine> m_AudioEngine{};
		TSharedPtr<TWorldRegistry> m_WorldRegistry{};

		TMaterialLoader m_MaterialLoader;
		TImageLoader m_ImageLoader;
		TMeshLoader m_MeshLoader;

		TRef<TGameWorld> m_CurrentWorld;
		
#ifdef ENABLE_EDITOR
		TRef<class EditorModule> m_Editor;
#endif

		TModuleStack m_ModuleStack{};
	};

	extern TSharedPtr<Engine> MakeRuntimeApplication(TApplicationLaunchParams const& launchParams);
}