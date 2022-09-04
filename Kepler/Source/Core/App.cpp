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
#include "Renderer/Pipelines/Default/DefaultUnlitPipeline.h"
#include "Renderer/Pipelines/ParamPack.h"
#include "Tools/ImageLoader.h"
#include "Renderer/Pipelines/Default/ScreenQuadPipeline.h"
#include "World/Game/GameEntity.h"
#include "Renderer/World/WorldTransform.h"
#include "Renderer/World/StaticMesh.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/World/WorldRenderer.h"
#include "Renderer/World/Camera.h"

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

		MainWindow = CHECKED(TPlatform::Get()->CreatePlatformWindow(1280, 720, "Kepler"));

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

		struct TWorldViewProj
		{
			matrix4x4 mViewProj = matrix4x4(1.0f);
			matrix4x4 mWorld = matrix4x4(1.0f);
		};

		struct TVertex
		{
			float3 Pos{};
			float3 Col{};
			float2 UV{};
		};

		TDynArray<TStaticMeshVertex> Vertices = {
			// Front
			{{-0.5f, -0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 0.5f, -0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.5f, -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			// Left
			{{-0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{-0.5f, -0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{-0.5f, -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			// Top
			{{-0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.5f, -0.5f,  0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.5f, -0.5f,  0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			// Right
			{{ 0.5f, -0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{ 0.5f, -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			// Bottom
			{{-0.5f, -0.5f, -0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 0.5f, -0.5f, -0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			// Back
			{{ 0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{-0.5f,  0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{-0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{ 0.5f,  0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		};

		TDynArray<TVertex> QuadVertices =
		{
			{{-1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		};

		TDynArray<u32> Indices = {
			0,1,3,1,2,3,
			4,5,7,5,6,7,
			8,9,11,9,10,11,
			12,13,15,13,14,15,
			16,17,19,17,18,19,
			20,21,23,21,22,23
		};

		TRef<TPipelineSamplerPack> Samplers;
		TRef<TParamBuffer> MvpBuffer;
		TRef<TImage2D> SampledImage;
		TRef<TGraphicsPipeline> UnlitPipeline;
		TRef<TTextureSampler2D> Sampler;

		TRef<TPipelineSamplerPack> QuadSamplers;
		TRef<TImage2D> QuadImage;

		TRef<TImage2D> DepthImage;
		TDynArray<TRef<TRenderTarget2D>> RenderTargets;
		TDynArray<TRef<TTextureSampler2D>> QuadSamplerHandles;
		TRef<TDepthStencilTarget2D> DepthTarget;
		TRef<TGraphicsPipeline> ScreenQuadPipeline;
		TRef<TVertexBuffer> QuadVertexBuffer;
		TRef<TIndexBuffer> QuadIndexBuffer;

		auto NewMesh = MeshLoader.LoadStaticMesh("Game://Sphere.gltf");

		auto Entity = CurrentWorld->CreateEntity("Entity");
		CurrentWorld->AddComponent<TStaticMeshComponent>(Entity, NewMesh);

		// TODO: Finish screen quad stuff
		auto RenderTask = TRenderThread::Submit(
			[&, this]
			{
				UnlitPipeline = MakeRef(New<TDefaultUnlitPipeline>());
				MvpBuffer = TParamBuffer::New(UnlitPipeline->GetParamMapping());
				// Screen quad
				QuadImage = TImage2D::New(1280, 720, EFormat::R8G8B8A8_UNORM, EImageUsage::ShaderResource | EImageUsage::RenderTarget, 1, 3);
				for (u32 Index = 0; Index < 3; ++Index)
				{
					RenderTargets.AppendBack(TRenderTarget2D::New(QuadImage, 0, Index));
					QuadSamplerHandles.AppendBack(TTextureSampler2D::New(QuadImage, 0, Index));
				}
				DepthImage = TImage2D::New(MainWindow->GetWidth(), MainWindow->GetHeight(), EFormat::D24_UNORM_S8_UINT, EImageUsage::DepthTarget);
				DepthTarget = TDepthStencilTarget2D::New(DepthImage);
				ScreenQuadPipeline = MakeRef(New<TScreenQuadPipeline>());
				QuadVertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(QuadVertices));
				QuadIndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Indices));
				QuadSamplers = ScreenQuadPipeline->GetParamMapping()->CreateSamplerPack();
			});
		Await(RenderTask);

		CurrentWorld->AddComponent<TMaterialComponent>(Entity, MaterialLoader.LoadMaterial("Engine://Materials/Mat_DefaultUnlit.kmat"));

		constexpr float3 Vec(7.0f, 1.0f, 0.0f);
		constexpr float4 Vec1(0.0f, 0.0f, 0.0f, 1.0f);
		float3 Result = glm::normalize(Vec * float3(Vec1) * 15.0f);

		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			float PositionX = 0.0f;

			while (Platform->HasActiveMainWindow())
			{
				KEPLER_PROFILE_FRAME("GameLoop")
					MainTimer.Begin();
				Platform->Update();

				if (!Platform->IsMainWindowMinimized() && Platform->HasActiveMainWindow())
				{
					// Update game state
					PositionX += GGlobalTimer->Delta();

					TRef<TMaterial> PlayerMaterial = CurrentWorld->GetComponent<TMaterialComponent>(Entity).GetMaterial();
					TCamera Camera(45.0f, (u32)MainWindow->GetWidth(), (u32)MainWindow->GetHeight(), 0.1f, 100.0f, float3(0.0f, -3.0f, 0.0f));
					PlayerMaterial->WriteCamera(Camera);

					float Width = (float)MainWindow->GetWidth();
					float Height = (float)MainWindow->GetHeight();
					Width = Width > 0 ? Width : 1;
					Height = Height > 0 ? Height : 1;

					TGameEntity& EntityRef = CurrentWorld->GetEntityFromId(Entity);

					auto Rotation = EntityRef.GetRotation();
					Rotation.z = PositionX * 100.0f;
					Rotation.x = PositionX * 100.0f;
					Rotation.y = PositionX * 100.0f;
					EntityRef.SetRotation(Rotation);

					CurrentWorld->UpdateWorld(GGlobalTimer->Delta(), EWorldUpdateKind::Game);

					// Render the frame
					TRenderThread::Submit([&, this]
						{
							TRef<TWorldRenderer> Renderer = TWorldRenderer::New(CurrentWorld, LowLevelRenderer);
							Renderer->Render({ 0, 0, (u32)MainWindow->GetWidth(), (u32)MainWindow->GetHeight() });
						});
					LowLevelRenderer->PresentAll();

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

		ModuleStack.Init();
	}

	void TApplication::TerminateModuleStack()
	{
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

		return false;
	}
}