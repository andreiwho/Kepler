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

	Engine::Engine(const TApplicationLaunchParams& launchParams)
	{
		KEPLER_INFO(LogApp, "Starting application initialization");
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
		m_WorldRenderer.Release();
		m_MeshLoader.ClearCache();
		m_ImageLoader.ClearCache();
		m_MaterialLoader.ClearLoadedMaterialCache();
		m_CurrentWorld.Release();

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
		m_CurrentWorld = m_WorldRegistry->CreateWorld<GameWorld>("MainWorld");

		// Begin main loop
		TTimer mainTimer{};
		GGlobalTimer = &mainTimer;
		float displayInfoTime = 0.0f;

		auto mainCamera = EntityHandle{ m_CurrentWorld, m_CurrentWorld->CreateCamera("Camera") };
		mainCamera->SetLocation(float3(0.0f, -3.0f, 1));
		mainCamera->SetRotation(float3(-20, 0.0f, 0.0f));
		RefPtr<ReflectedClass> refClass = GetReflectedClass<CameraComponent>();
		if (refClass)
		{
			KEPLER_INFO(LogApp, "Found reflected class {}", refClass->GetName());
			auto& field = refClass->GetFieldByName("RenderTargetName");
			String* value = field.GetValueFor<String>(mainCamera.GetComponent<CameraComponent>());
			KEPLER_INFO(LogApp, "Render target name for main camera is: {}", *value);
		}

		auto ambientLight = EntityHandle{ m_CurrentWorld, m_CurrentWorld->CreateEntity("AmbientLight") };
		AmbientLightComponent* pALC = ambientLight.AddComponent<AmbientLightComponent>();
		pALC->SetColor(float3(0.3f, 0.3f, 0.3f));
		ambientLight->SetLocation(float3(-2.0f, 0.0f, 0.0f));
		
		auto dirLight = EntityHandle{ m_CurrentWorld, m_CurrentWorld->CreateEntity("Directional Light") };
		DirectionalLightComponent* pDLC = dirLight.AddComponent<DirectionalLightComponent>();
		pDLC->SetColor(float3(1.0f, 1.0f, 1.0f));
		pDLC->SetIntensity(1.0f);
		dirLight->SetRotation(float3(-135, 0, 90.0f));
		dirLight->SetLocation(float3(-3, 0.0f, 0.0f));

		auto mesh = m_MeshLoader.LoadStaticMesh("Game://LP.fbx", true);
		i32 x = 0;
		i32 y = 0;
		for (i32 idx = 0; idx < 10; ++idx)
		{
			if (x > 3)
			{
				x = 0;
				y++;
			}

			auto entity = EntityHandle{ m_CurrentWorld, m_CurrentWorld->CreateEntity(fmt::format("Entity{}", idx)) };
			entity.AddComponent<StaticMeshComponent>(mesh);
			entity.AddComponent<MaterialComponent>(m_MaterialLoader.LoadMaterial("Engine://Materials/Mat_DefaultLit.kmat"));
			entity->SetScale(float3(0.3f));
			entity->SetRotation(float3(0, 0.0f, (float)(rand() % 360)));
			entity->SetLocation(float3(x, y, 0.0f));

			x++;
		}

		if (TPlatform* pPlatform = TPlatform::Get())
		{
			pPlatform->RegisterPlatformEventListener(this);
			float posX = 0.0f;
#ifdef ENABLE_EDITOR
			m_Editor->SetEditedWorld(m_CurrentWorld);
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
					m_WorldRenderer->InitFrame(m_CurrentWorld);
					m_WorldRenderer->UpdateRendererMainThread(mainTimer.Delta());
					m_CurrentWorld->UpdateWorld(GGlobalTimer->Delta(), EWorldUpdateKind::Game);

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

				mainTimer.End();

#ifdef ENABLE_DEBUG
				if (pPlatform->HasActiveMainWindow())
				{
					displayInfoTime += mainTimer.Delta();
					if (displayInfoTime >= 1.0f)
					{
						displayInfoTime = 0;
						m_MainWindow->SetTitle(fmt::format("{} <{}>", m_MainWindow->GetName(), 1.0f / mainTimer.Delta()));
					}
				}
#endif
			}
		}
		TerminateModuleStack();
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
		return false;
	}
}