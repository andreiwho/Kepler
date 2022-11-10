#include "App.h"
#include "Log.h"

#include "Platform/Platform.h"

#include <iostream>
#include "Timer.h"
#include "Async/Async.h"

// Test
#include "Renderer/HLSLShaderCompiler.h"
#include "Core/Filesystem/FileUtils.h"
#include "Renderer/Elements/IndexBuffer.h"
#include "Renderer/Elements/HLSLShader.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "Renderer/Pipelines/ParamPack.h"
#include "Tools/ImageLoader.h"
#include "World/Game/GameEntity.h"
#include "Renderer/World/WorldTransform.h"
#include "Renderer/World/StaticMesh.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/World/WorldRenderer.h"
#include "Renderer/World/Camera.h"
#include "Editor/EditorModule.h"
#include "imgui.h"
#include "Editor/Widgets/Elements.h"
#include "World/Game/Components/TransformComponent.h"
#include "World/Game/Helpers/EntityHelper.h"
#include "World/Game/Components/Light/AmbientLightComponent.h"
#include "World/Game/Components/Light/DirectionalLightComponent.h"
#include "Renderer/Subrenderer/Subrenderer2D.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/GameWorldSerializer.h"
#include "Platform/Input.h"

namespace ke
{
	TCommandLineArguments::TCommandLineArguments(Array<String> const& cmdLine)
	{
		// Parse command line args
		// Game module name must always be the first arg
		CHECKMSG(cmdLine.GetLength() > 0, "The first param of the command line must be the application directory");
#ifdef ENABLE_EDITOR
		GameModuleDirectory = cmdLine[0];
#else
		GameModuleDirectory = RELEASE_APPLICATION_NAME;
#endif
		usize idx = 0;
		for (const auto& arg : cmdLine)
		{
			if (idx == 0)
			{
				continue;
				idx++;
			}

			// Do someting 
			// ...
			// Increment the index
			idx++;
		}
	}

	Engine* Engine::Instance = nullptr;

	Engine::Engine(const TApplicationLaunchParams& launchParams)
	{
		KEPLER_INFO(LogApp, "Starting application initialization");
		Instance = this;

#ifndef ENABLE_EDITOR
		CurrentWorldState = EWorldUpdateKind::Play;
#else
		CurrentWorldState = EWorldUpdateKind::Edit;
#endif

		InitVFSAliases(launchParams);
		m_ReflectionDatabase.FillReflectionDatabaseEntries();

		m_AssetManager = MakeShared<AssetManager>();

		TWindowParams windowParams{};
		windowParams.bMaximized = false;
		m_MainWindow = CHECKED(TPlatform::Get()->CreatePlatformWindow(1920, 1080, "Kepler", windowParams));

		m_LowLevelRenderer = MakeShared<LowLevelRenderer>();
		m_LowLevelRenderer->InitRenderStateForWindow(m_MainWindow);
		m_AudioEngine = AudioEngine::CreateAudioEngine(EAudioEngineAPI::Default);
		// AudioEngine->Play("Game://Startup.mp3");

		m_WorldRegistry = MakeShared<WorldRegistry>();

		m_WorldRenderer = Await(TRenderThread::Submit([this] { return WorldRenderer::New(); }));
		m_WorldRenderer->PushSubrenderer<Subrenderer2D, ESubrendererOrder::Overlay>();
	}

	void Engine::InitVFSAliases(const TApplicationLaunchParams& launchParams)
	{
		// Initialize VFS
		VFSRegisterPathAlias("Engine", "Kepler/Assets");
		VFSRegisterPathAlias("EngineShaders", "Kepler/Shaders");
		VFSRegisterPathAlias("Game", fmt::format("{}/Assets", launchParams.CommandLine.GameModuleDirectory));
	}

	Engine::~Engine()
	{
		GameWorld::ClearStaticState();
		m_WorldRenderer.Release();
		m_MeshLoader.ClearCache();
		m_ImageLoader.ClearCache();
		m_MaterialLoader.ClearLoadedMaterialCache();
		CurrentWorld.Release();

		m_WorldRegistry.reset();
		m_AudioEngine.reset();
		m_LowLevelRenderer.reset();
		m_AssetManager.reset();
		KEPLER_INFO(LogApp, "Finishing application termination");
	}

	void Engine::Run()
	{
		KEPLER_INFO(LogApp, "Application Run called...");

		InitApplicationModules();

		// Create the world
		CurrentWorld = m_WorldRegistry->CreateWorld<GameWorld>("MainWorld");

		// Begin main loop
		TTimer mainTimer{};
		GGlobalTimer = &mainTimer;
		float displayInfoTime = 0.0f;

		if (TPlatform* pPlatform = TPlatform::Get())
		{
			pPlatform->RegisterPlatformEventListener(this);
			float posX = 0.0f;
#ifdef ENABLE_EDITOR
			m_Editor->SetEditedWorld(CurrentWorld);
#endif

			m_ModuleStack.OnPostWorldInit();

			while (pPlatform->HasActiveMainWindow())
			{
				KEPLER_PROFILE_FRAME("GameLoop");
				mainTimer.Begin();
				pPlatform->Update();

				if (!pPlatform->IsMainWindowMinimized() && pPlatform->HasActiveMainWindow())
				{
#if ENABLE_EDITOR
					const float2 vpSize = m_Editor->GetViewportSize(EViewportIndex::Viewport1);
#else
					const float2 vpSize = float2(m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
#endif
					// Initialize the renderer
					m_WorldRenderer->InitFrame(CurrentWorld);
					m_WorldRenderer->UpdateRendererMainThread(mainTimer.Delta());

					CurrentWorld->UpdateWorld(GGlobalTimer->Delta(), CurrentWorldState);
					// Render the world
					auto renderTask = TRenderThread::Submit([&, this]
						{
							m_WorldRenderer->Render({ 0, 0, (u32)vpSize.x, (u32)vpSize.y });
						});
					m_ModuleStack.OnUpdate(GGlobalTimer->Delta());
#ifdef ENABLE_EDITOR
					//Await(renderTask);

					m_Editor->BeginGUIPass();
					m_Editor->DrawEditor();
					m_ModuleStack.OnRenderGUI();
					m_Editor->EndGUIPass();
#else
					(void)renderTask;
#endif
					m_WorldRenderer->ClearSubrenderersState();
					m_LowLevelRenderer->PresentAll();

					if (pPlatform->IsMainWindowUnfocused())
					{
						using namespace std::chrono_literals;
						std::this_thread::sleep_for(20ms);
					}
				}
				else // minimized
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(10ms);
				}

				FlushMainThreadTasks();
				CheckWorldUpdated();
				mainTimer.End();
				FrameTime = 1.0f / mainTimer.Delta();
				FramesPerSecond = mainTimer.Delta() * 1000.0f;
			}
		}
		TerminateModuleStack();
	}

	void Engine::SetMainWorld(RefPtr<GameWorld> newWorld)
	{
		CurrentWorld = newWorld;
		m_bWorldUpdated = true;
	}

	void Engine::OnCurrentWorldStateChange(EWorldUpdateKind newUpdateKind)
	{
		// World state changed
		if (newUpdateKind == CurrentWorldState)
		{
			return;
		}

		switch (newUpdateKind)
		{
		case EWorldUpdateKind::Play:
		{
			GameWorldSerializer serializer{ CurrentWorld };
			auto serializedData = serializer.Serialize();
			GameWorldDeserializer deserializer;
			auto pPlayWorld = deserializer.Deserialize("PlayWorld", serializedData, PLAY_INDEX);
			SetMainWorld(pPlayWorld);
		}
		break;
		case EWorldUpdateKind::Edit:
		{
			if (CurrentWorld)
			{
				WorldRegistry::Get()->DestroyWorld(CurrentWorld);
				if (auto pWorld = WorldRegistry::Get()->GetWorldAt(EDIT_INDEX))
				{
					SetMainWorld(RefCast<GameWorld>(pWorld));
				}
			}
		}
		break;
		default:
			break;
		}
	}

	void Engine::OnPlatformEvent(const TPlatformEventBase& event)
	{
		TPlatformEventDispatcher dispatcher{ event };
		dispatcher.Dispatch(this, &Engine::OnWindowClosed);
		dispatcher.Dispatch(this, &Engine::OnWindowResized);
		dispatcher.Dispatch(this, &Engine::OnKeyDown);

		if (!event.Handled)
		{
			m_ModuleStack.HandlePlatformEvent(event);
		}
	}

	void Engine::InitApplicationModules()
	{
		// Initialize engine modules
		// ...

		ChildSetupModuleStack(m_ModuleStack);

#ifdef ENABLE_EDITOR
		m_Editor = MakeRef(New<EditorModule>(m_MainWindow));
		m_ModuleStack.PushModule(m_Editor, EModulePushStrategy::Overlay);
		TEditorElements::SetMainWindow(m_MainWindow);
#endif

		m_ModuleStack.Init();
	}

	void Engine::TerminateModuleStack()
	{
#ifdef ENABLE_EDITOR
		m_Editor.Release();
#endif
		m_ModuleStack.Terminate();
		m_ModuleStack.Clear();
	}

	bool Engine::OnWindowClosed(const TWindowClosedEvent& event)
	{
		if (m_LowLevelRenderer)
		{
			m_LowLevelRenderer->DestroyRenderStateForWindow(event.Window);
			return true;
		}
		return false;
	}

	bool Engine::OnWindowResized(const TWindowSizeEvent& event)
	{
		if (m_LowLevelRenderer)
		{
			m_LowLevelRenderer->OnWindowResized(event.Window);
			return true;
		}
		return false;
	}

	bool Engine::OnKeyDown(const TKeyDownEvent& event)
	{
		if (event.Key == EKeyCode::Escape && CurrentWorldState == EWorldUpdateKind::Play)
		{
			m_bExitPlayRequested = true;
		}

		if (event.Key == EKeyCode::F5)
		{
			OnCurrentWorldStateChange(EWorldUpdateKind::Play);
			CurrentWorldState = EWorldUpdateKind::Play;
		}

		return false;
	}

	void Engine::CheckWorldUpdated()
	{
		if (m_bExitPlayRequested)
		{
			OnCurrentWorldStateChange(EWorldUpdateKind::Edit);
			CurrentWorldState = EWorldUpdateKind::Edit;
			m_bExitPlayRequested = false;
		}

		if (m_bWorldUpdated)
		{
			m_bWorldUpdated = false;
			m_Editor->SetEditedWorld(CurrentWorld);

			m_ModuleStack.OnPostWorldInit();
		}
	}

	void Engine::FlushMainThreadTasks()
	{
		std::function<void()> task;
		while (m_MainTasks.Dequeue(task))
		{
			if (task)
			{
				task();
			}
		}
	}

	TestMovementComponent::TestMovementComponent()
	{
		m_PerInstanceOffset = (float)(rand() % 256);
	}

	void TestMovementComponent::Update(float deltaTime)
	{
		if (m_MovementType == EMovementType::Dynamic)
		{
			EntityHandle handle{ GetWorld(), GetOwner() };
			m_MovementValue = glm::sin(m_CurrentTime + m_PerInstanceOffset);
			auto location = handle->GetLocation();
			location.z = m_MovementValue * m_Speed;
			handle->SetLocation(location);
			m_CurrentTime += deltaTime;

			auto rotation = handle->GetRotation();
			rotation.z += deltaTime * m_PerInstanceOffset;
			rotation.x = rotation.z;
			handle->SetRotation(rotation);
		}
	}

}