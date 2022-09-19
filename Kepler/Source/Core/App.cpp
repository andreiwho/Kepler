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

namespace Kepler
{
	TCommandLineArguments::TCommandLineArguments(TDynArray<TString> const& CommandLine)
	{
		// Parse command line args
		// Game module name must always be the first arg
		CHECKMSG(CommandLine.GetLength() > 0, "The first param of the command line must be the application directory");
		GameModuleDirectory = CommandLine[0];

		usize Index = 0;
		for (const auto& Argument : CommandLine)
		{
			if (Index == 0)
			{
				continue;
				Index++;
			}
			// Do someting 
			// ...
			// Increment the index
			Index++;
		}
	}

	TApplication::TApplication(const TApplicationLaunchParams& LaunchParams)
	{
		KEPLER_INFO(LogApp, "Starting application initialization");
		InitVFSAliases(LaunchParams);

		TWindowParams WindowParams{};
		WindowParams.bMaximized = false;
		MainWindow = CHECKED(TPlatform::Get()->CreatePlatformWindow(1280, 720, "Kepler", WindowParams));

		LowLevelRenderer = MakeShared<TLowLevelRenderer>();
		LowLevelRenderer->InitRenderStateForWindow(MainWindow);
		AudioEngine = TAudioEngine::CreateAudioEngine(EAudioEngineAPI::Default);
		// AudioEngine->Play("Game://Startup.mp3");

		WorldRegistry = MakeShared<TWorldRegistry>();
	}

	void TApplication::InitVFSAliases(const TApplicationLaunchParams& LaunchParams)
	{
		// Initialize VFS
		VFSRegisterPathAlias("Engine", "Kepler/Assets");
		VFSRegisterPathAlias("EngineShaders", "Kepler/Shaders");
		VFSRegisterPathAlias("Game", fmt::format("{}/Assets", LaunchParams.CommandLine.GameModuleDirectory));
	}

	TApplication::~TApplication()
	{
		MeshLoader.ClearCache();
		ImageLoader.ClearCache();
		MaterialLoader.ClearLoadedMaterialCache();
		CurrentWorld.Release();

		WorldRegistry.reset();
		AudioEngine.reset();
		LowLevelRenderer.reset();
		KEPLER_INFO(LogApp, "Finishing application termination");
	}

	void TApplication::Run()
	{
		KEPLER_INFO(LogApp, "Application Run called...");

		InitApplicationModules();

		// Create the world
		CurrentWorld = WorldRegistry->CreateWorld<TGameWorld>("MainWorld");

		TWorldTransform Transform;

		// Begin main loop
		TTimer MainTimer{};
		GGlobalTimer = &MainTimer;
		float DisplayInfoTime = 0.0f;

		auto MainCamera = TEntityHandle{ CurrentWorld, CurrentWorld->CreateCamera("Camera") };
		MainCamera->SetLocation(float3(0.0f, -3.0f, 1));
		MainCamera->SetRotation(float3(-20, 0.0f, 0.0f));

		auto MeshSections = MeshLoader.LoadStaticMeshSections("Game://LP.fbx", false);
		i32 X = 0;
		i32 Y = 0;
		for (i32 Index = 0; Index < 100; ++Index)
		{
			if (X > 10)
			{
				X = 0;
				Y++;
			} 

			auto Entity = TEntityHandle{ CurrentWorld, CurrentWorld->CreateEntity(fmt::format("Entity{}", Index)) };
			Entity.AddComponent<TStaticMeshComponent>(MeshSections);
			Entity.AddComponent<TMaterialComponent>(MaterialLoader.LoadMaterial("Engine://Materials/Mat_DefaultUnlit.kmat"));
			Entity->SetScale(float3(3.0f));
			Entity->SetRotation(float3(0, 0.0f, (float)(rand() % 360)));
			Entity->SetLocation(float3(X, Y, 0.0f));

			X++;
		}

		constexpr float3 Vec(7.0f, 1.0f, 0.0f);
		constexpr float4 Vec1(0.0f, 0.0f, 0.0f, 1.0f);
		float3 Result = glm::normalize(Vec * float3(Vec1) * 15.0f);

		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			float PositionX = 0.0f;
#ifdef ENABLE_EDITOR
			Editor->SetEditedWorld(CurrentWorld);
#endif

			while (Platform->HasActiveMainWindow())
			{
				KEPLER_PROFILE_FRAME("GameLoop");
				MainTimer.Begin();
				Platform->Update();

				if (!Platform->IsMainWindowMinimized() && Platform->HasActiveMainWindow())
				{
					// Update game state
					// PositionX += GGlobalTimer->Delta();

#if ENABLE_EDITOR
					const float2 ViewportSize = Editor->GetViewportSize(EViewportIndex::Viewport1);
#else
					const float2 ViewportSize = float2(MainWindow->GetWidth(), MainWindow->GetHeight());
#endif
					CurrentWorld->UpdateWorld(GGlobalTimer->Delta(), EWorldUpdateKind::Game);
					ModuleStack.OnUpdate(GGlobalTimer->Delta());

					// Render the frame
					// We are not waiting here, because we also want the editor GUI to be drawn as well. 
					// This is a subject to consider though
					auto RenderTask = TRenderThread::Submit([&, this]
						{
							TRef<TWorldRenderer> Renderer = TWorldRenderer::New(CurrentWorld, LowLevelRenderer);
							Renderer->Render({ 0, 0, (u32)ViewportSize.x, (u32)ViewportSize.y });
						});
#ifdef ENABLE_EDITOR
					Await(RenderTask);

					Editor->BeginGUIPass();
					Editor->DrawEditor();
					ModuleStack.OnRenderGUI();
					Editor->EndGUIPass();
#else
					(void)RenderTask;
#endif
					LowLevelRenderer->PresentAll();
					
					if (Platform->IsMainWindowUnfocused())
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

				MainTimer.End();

#ifdef ENABLE_DEBUG
				if (Platform->HasActiveMainWindow())
				{
					DisplayInfoTime += MainTimer.Delta();
					if (DisplayInfoTime >= 1.0f)
					{
						DisplayInfoTime = 0;
						MainWindow->SetTitle(fmt::format("{} <{}>", MainWindow->GetName(), 1.0f / MainTimer.Delta()));
					}
					// KEPLER_TRACE(LogApp, "Loop time: {}", 1.0f / MainTimer.Delta());
				}
#endif
			}
		}

		TerminateModuleStack();
	}

	void TApplication::OnPlatformEvent(const TPlatformEventBase& Event)
	{
		TPlatformEventDispatcher Dispatcher{ Event };
		Dispatcher.Dispatch(this, &TApplication::OnWindowClosed);
		Dispatcher.Dispatch(this, &TApplication::OnWindowResized);
		Dispatcher.Dispatch(this, &TApplication::OnKeyDown);

		if (!Event.Handled)
		{
			ModuleStack.HandlePlatformEvent(Event);
		}
	}

	void TApplication::InitApplicationModules()
	{
		// Initialize engine modules
		// ...

		ChildSetupModuleStack(ModuleStack);

#ifdef ENABLE_EDITOR
		Editor = MakeRef(New<TEditorModule>(MainWindow));
		ModuleStack.PushModule(Editor, EModulePushStrategy::Overlay);
		TEditorElements::SetMainWindow(MainWindow);
#endif

		ModuleStack.Init();
	}

	void TApplication::TerminateModuleStack()
	{
#ifdef ENABLE_EDITOR
		Editor.Release();
#endif
		ModuleStack.Terminate();
		ModuleStack.Clear();
	}

	bool TApplication::OnWindowClosed(const TWindowClosedEvent& Event)
	{
		if (LowLevelRenderer)
		{
			LowLevelRenderer->DestroyRenderStateForWindow(Event.Window);
			return true;
		}
		return false;
	}

	bool TApplication::OnWindowResized(const TWindowSizeEvent& Event)
	{
		if (LowLevelRenderer)
		{
			LowLevelRenderer->OnWindowResized(Event.Window);
			return true;
		}
		return false;
	}

	bool TApplication::OnKeyDown(const TKeyDownEvent& Event)
	{
		/*
		if (Event.Key == EKeyCode::Space)
		{
			TRef<TSound> Sound = AudioEngine->GetOrLoadSound("Game://Coin.wav");
			Sound->Play();
		}

		if (Event.Key == EKeyCode::Escape)
			AudioEngine->Play("Game://cool.flac", ESoundCreateFlags::Streamed);

		if (Event.Key == EKeyCode::F)
			AudioEngine->Play("Game://prog3.mp3", ESoundCreateFlags::Streamed);

		if (Event.Key == EKeyCode::Z)
			AudioEngine->UnloadPlaybackCache(true);
			*/
		return false;
	}
}