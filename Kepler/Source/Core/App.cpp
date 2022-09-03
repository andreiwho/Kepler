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

		auto Entity = CurrentWorld->CreateEntity("Entity");
		CurrentWorld->AddComponent<TStaticMeshComponent>(Entity, Vertices, Indices);

		// TODO: Finish screen quad stuff
		auto RenderTask = TRenderThread::Submit(
			[&, this]
			{
				UnlitPipeline = MakeRef(New<TDefaultUnlitPipeline>());
				MvpBuffer = TParamBuffer::New(UnlitPipeline->GetParamMapping());
				auto ImageData = Await(TImageLoader::Load("Game://Ground.png"));
				SampledImage = TImage2D::New(ImageData.Width, ImageData.Height, EFormat::R8G8B8A8_UNORM, EImageUsage::ShaderResource);
				SampledImage->Write(LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList(), 0, 0, ImageData.Width, ImageData.Height, ImageData.Data);
				Sampler = TTextureSampler2D::New(SampledImage, 0, 0);
				Samplers = UnlitPipeline->GetParamMapping()->CreateSamplerPack();
				Samplers->Write("Albedo", Sampler);

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

		CurrentWorld->AddComponent<TMaterialComponent>(Entity, UnlitPipeline);

		constexpr float3 Vec(7.0f, 1.0f, 0.0f);
		constexpr float4 Vec1(0.0f, 0.0f, 0.0f, 1.0f);
		float3 Result = glm::normalize(Vec * float3(Vec1) * 15.0f);

		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			float PositionX = 0.0f;

			while (Platform->HasActiveMainWindow())
			{
				MainTimer.Begin();
				Platform->Update();

				if (!Platform->IsMainWindowMinimized() && Platform->HasActiveMainWindow())
				{
					// Update game state

					CurrentWorld->UpdateWorld(GGlobalTimer->Delta(), EWorldUpdateKind::Game);

					PositionX += GGlobalTimer->Delta();

					TRef<TMaterial> PlayerMaterial = CurrentWorld->GetComponent<TMaterialComponent>(Entity).GetMaterial();
					auto& Param = PlayerMaterial->GetParamReferenceForWriting<matrix4x4>("mViewProj");

					float Width = (float)MainWindow->GetWidth();
					float Height = (float)MainWindow->GetHeight();
					Width = Width > 0 ? Width : 1;
					Height = Height > 0 ? Height : 1;
					auto Projection = glm::perspectiveFovRH_ZO(glm::radians(45.0f), (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.1f, 100.0f);
					auto View = glm::lookAtRH(float3(0.0f, -3.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));

					TGameEntity& EntityRef = CurrentWorld->GetEntityFromId(Entity);

					auto Rotation = EntityRef.GetRotation();
					Rotation.z = PositionX * 100.0f;
					Rotation.x = PositionX * 100.0f;
					Rotation.y = PositionX * 100.0f;
					EntityRef.SetRotation(Rotation);

					auto& TransformParam = PlayerMaterial->GetParamReferenceForWriting<matrix4x4>("mWorld");
					TransformParam = glm::transpose(EntityRef.GetTransform().GenerateWorldMatrix());

					// Param = Param * glm::translate(glm::identity<matrix4x4>(), float3(0.0f, glm::sin(PositionX), 0.0f));
					Param = glm::transpose(Projection * View);
					PlayerMaterial->WriteSampler("Albedo", Sampler);

					// Render the frame
					TRenderThread::Submit([&, this]
						{
							TRef<TWorldRenderer> Renderer = TWorldRenderer::New(CurrentWorld, LowLevelRenderer);
							Renderer->Render({ 0, 0, (u32)MainWindow->GetWidth(), (u32)MainWindow->GetHeight() });
						});
					LowLevelRenderer->PresentAll();

					/*
					TRenderThread::Submit(
						[&, this]
						{
							static u32 FrameIndex = 0;

							auto pImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
							auto SwapChain = LowLevelRenderer->GetSwapChain(0);

							// Update all materials
							CurrentWorld->GetComponentView<TMaterialComponent>().each(
								[pImmList](auto, TMaterialComponent& Component)
								{
									Component.GetMaterial()->RT_Update(pImmList);
								});
							// MvpBuffer->RT_UploadToGPU(pImmList);

							// Render
							if (SwapChain)
							{
								// TODO: Move this into high level scene renderer
								// - Pipeline sorting
								pImmList->BeginDebugEvent("Clear Render State");
								pImmList->ClearSamplers();
								pImmList->EndDebugEvent();

								// MESH PASS
								pImmList->BeginDebugEvent("Mesh Pass");
								// Write to quad render target
								pImmList->StartDrawingToRenderTargets(RenderTargets[FrameIndex], DepthTarget);
								pImmList->ClearDepthTarget(DepthTarget);
								pImmList->ClearRenderTarget(RenderTargets[FrameIndex], { 0.1f, 0.1f, 0.1f, 1.0f });
								pImmList->SetViewport(0, 0, (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.0f, 1.0f);
								pImmList->SetScissor(0, 0, (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight());

								CurrentWorld->GetComponentView<TMaterialComponent, TStaticMeshComponent>().each(
									[pImmList](auto, TMaterialComponent& MT, TStaticMeshComponent& SM)
									{
										pImmList->BindParamBuffers(MT.GetMaterial()->GetParamBuffer(), 0);
										pImmList->BindVertexBuffers(SM.GetStaticMesh()->GetVertexBuffer(), 0, 0);
										pImmList->BindIndexBuffer(SM.GetStaticMesh()->GetIndexBuffer(), 0);
										pImmList->BindPipeline(MT.GetMaterial()->GetPipeline());
										pImmList->BindSamplers(MT.GetMaterial()->GetSamplers());
										pImmList->DrawIndexed(SM.GetStaticMesh()->GetIndexCount(), 0, 0);
									}
								);
								pImmList->EndDebugEvent();
								// END MESH PASS


								pImmList->BeginDebugEvent("Screen Quad Pass");
								pImmList->StartDrawingToSwapChainImage(SwapChain);
								pImmList->ClearSwapChainImage(SwapChain, {0.1f, 0.1f, 0.1f, 1.0f});
								pImmList->BindVertexBuffers(QuadVertexBuffer, 0, 0);
								pImmList->BindIndexBuffer(QuadIndexBuffer, 0);
								pImmList->BindPipeline(ScreenQuadPipeline);

								//Write quad sampler
								QuadSamplers->Write("RenderTarget", QuadSamplerHandles[FrameIndex]);
								// and
								pImmList->BindSamplers(QuadSamplers);
								pImmList->DrawIndexed(QuadIndexBuffer->GetCount(), 0, 0);
								pImmList->EndDebugEvent();

								FrameIndex = (FrameIndex + 1) % 3;
							}
						});
					LowLevelRenderer->PresentAll();
					*/
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