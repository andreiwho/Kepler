#pragma once
#include "Core/Types.h"
#include "Platform/Window.h"
#include "Platform/PlatformEvent.h"
#include "Core/Malloc.h"
#include "Renderer/RenderThread.h"
#include "Renderer/LowLevelRenderer.h"
#include "Tools/MaterialLoader.h"
#include "Renderer/World/WorldRenderer.h"

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
#include "Filesystem/AssetSystem/AssetManager.h"
#include "Reflection/ReflectionDatabase.h"
#include "App.gen.h"

namespace ke
{
#define EDIT_INDEX 0
#define PLAY_INDEX 1

	reflected enum class EMovementType
	{
		Static,
		Dynamic,
	};

	reflected class TestMovementComponent : public NativeScriptComponent
	{
		reflected_body();
	public:
		TestMovementComponent();

		reflected EMovementType m_MovementType = EMovementType::Dynamic;
		
		reflected kmeta(readonly)
		float m_MovementValue = 0.0f;

		reflected kmeta(editspeed=0.01f)
		float m_Speed = 1.0f;

		void Update(float deltaTime);

	private:
		float m_PerInstanceOffset = 0.0f;
		float m_CurrentTime = 0.0f;
	};

	struct TCommandLineArguments
	{
		TCommandLineArguments() = default;
		TCommandLineArguments(Array<String> const& cmdLine);

		String GameModuleDirectory = "";
	};

	struct TApplicationLaunchParams
	{
		TCommandLineArguments CommandLine;
	};

	// --------------------------------------------
	// This is an important class.
	// - All of the internal initialization and application logic will happen inside the Engine::Run function
	// --------------------------------------------
	reflected class Engine : public IPlatformEventListener
	{
		reflected_body();
		static Engine* Instance;
	public:
		Engine()
		{
			// Disallow default construction
			CRASH();
		}

		Engine(const TApplicationLaunchParams& launchParams);
		virtual ~Engine();

		virtual void Run();
		static Engine* Get() { return Instance; }

		void SetMainWorld(RefPtr<GameWorld> newWorld);

		reflected kmeta(prechange = OnCurrentWorldStateChange)
		EWorldUpdateKind CurrentWorldState{};
		void OnCurrentWorldStateChange(EWorldUpdateKind newUpdateKind);

		reflected RefPtr<GameWorld> CurrentWorld;

		reflected kmeta(readonly, assettype = Map)
		AssetTreeNode* WorldAsset{};

		reflected kmeta(readonly)
		float FrameTime = 0.0f;

		reflected kmeta(readonly)
		float FramesPerSecond = 0.0f;

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
		void CheckWorldUpdated();

	private:
		ReflectionDatabase m_ReflectionDatabase{};
		TWindow* m_MainWindow{};
		SharedPtr<LowLevelRenderer> m_LowLevelRenderer{};
		SharedPtr<AudioEngine> m_AudioEngine{};
		SharedPtr<WorldRegistry> m_WorldRegistry{};
		SharedPtr<AssetManager> m_AssetManager{};

		TMaterialLoader m_MaterialLoader;
		TImageLoader m_ImageLoader;
		MeshLoader m_MeshLoader;

		RefPtr<WorldRenderer> m_WorldRenderer;

		
#ifdef ENABLE_EDITOR
		RefPtr<class EditorModule> m_Editor;
#endif

		TModuleStack m_ModuleStack{};

		bool m_bWorldUpdated = false;
		bool m_bExitPlayRequested = false;
	};

	extern SharedPtr<Engine> MakeRuntimeApplication(TApplicationLaunchParams const& launchParams);
}