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

namespace Kepler
{
	TCommandLineArguments::TCommandLineArguments(TDynArray<TString> const& CommandLine)
	{
		// Parse command line args
		for (const TString& arg : CommandLine)
		{
			// TODO: Do some parsing
		}
	}

	TApplication::TApplication(const TApplicationLaunchParams& LaunchParams)
	{
		KEPLER_INFO(LogApp, "Starting application initialization");

		MainWindow = CHECKED(TPlatform::Get()->CreatePlatformWindow(1280, 720, "Kepler"));

		LowLevelRenderer = MakeShared<TLowLevelRenderer>();
		LowLevelRenderer->InitRenderStateForWindow(MainWindow);
		AudioEngine = TAudioEngine::CreateAudioEngine(EAudioEngineAPI::Default);
	}

	TApplication::~TApplication()
	{
		AudioEngine.reset();
		LowLevelRenderer.reset();
		KEPLER_INFO(LogApp, "Finishing application termination");
	}

	void TApplication::Run()
	{
		KEPLER_INFO(LogApp, "Application Run called...");

		InitApplicationModules();

		// Begin main loop
		TTimer MainTimer{};
		GGlobalTimer = &MainTimer;
		float DisplayInfoTime = 0.0f;

		struct TWorldViewProj
		{
			matrix4x4 mWorldViewProj = matrix4x4(1.0f);
		};

		struct TVertex
		{
			float3 Pos{};
			float3 Col{};
			float2 UV{};
		};

		TDynArray<TVertex> Vertices = {
			{{-0.5f, 0.0f,  0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 0.5f, 0.0f,  0.5f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.5f, 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.5f, 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		};

		TDynArray<TVertex> QuadVertices =
		{
			{{-1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{-1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		};

		TDynArray<u32> Indices = { 0,1,3,1,2,3 };

		TRef<TPipelineParamMapping> Mapping = MakeRef(New<TPipelineParamMapping>());
		Mapping->AddParam("mWorldViewProj", 0, 0, EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);
		Mapping->AddTextureSampler("Albedo", EShaderStageFlags::Pixel, 0);
		
		TRef<TPipelineSamplerPack> Samplers = Mapping->CreateSamplerPack();
		TRef<TParamBuffer> MvpBuffer;
		TRef<TImage2D> SampledImage;
		TRef<TVertexBuffer> VertexBuffer;
		TRef<TIndexBuffer> IndexBuffer;
		TRef<TGraphicsPipeline> UnlitPipeline;
		TRef<TTextureSampler2D> Sampler;

		// Screen quad stuff
		TRef<TPipelineParamMapping> ScreenQuadMapping = MakeRef(New<TPipelineParamMapping>());
		ScreenQuadMapping->AddTextureSampler("RenderTarget", EShaderStageFlags::Pixel, 0);

		TRef<TPipelineSamplerPack> QuadSamplers = ScreenQuadMapping->CreateSamplerPack();
		TRef<TImage2D> QuadImage;

		TRef<TImage2D> DepthImage;
		TDynArray<TRef<TRenderTarget2D>> RenderTargets;
		TDynArray<TRef<TTextureSampler2D>> QuadSamplerHandles;
		TRef<TDepthStencilTarget2D> DepthTarget;
		TRef<TGraphicsPipeline> ScreenQuadPipeline;
		TRef<TVertexBuffer> QuadVertexBuffer;
		TRef<TIndexBuffer> QuadIndexBuffer;
		
		// TODO: Finish screen quad stuff

		TRef<TDataBlob> VertexBlob = TDataBlob::New(Vertices);
		auto RenderTask = TRenderThread::Submit(
			[&, this]
			{
				MvpBuffer = TParamBuffer::New(Mapping);
				VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Vertices));
				IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Indices));
				UnlitPipeline = MakeRef(New<TDefaultUnlitPipeline>());
				auto ImageData = Await(TImageLoader::LoadImage("Engine/Ground.png"));
				SampledImage = TImage2D::New(ImageData.Width, ImageData.Height, EFormat::R8G8B8A8_UNORM, EImageUsage::ShaderResource);
				SampledImage->Write(LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList(), 0, 0, ImageData.Width, ImageData.Height, ImageData.Data);
				Sampler = TTextureSampler2D::New(SampledImage, 0, 0);
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
			});

		constexpr float3 Vec(7.0f, 1.0f, 0.0f);
		constexpr float4 Vec1(0.0f, 0.0f, 0.0f, 1.0f);
		float3 Result = glm::normalize(Vec * float3(Vec1) * 15.0f);

		const TString InitialWindowName = MainWindow->GetTitle();
		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			float PositionX = 0.0f;

			Await(RenderTask);
			while (Platform->HasActiveMainWindow())
			{
				MainTimer.Begin();
				Platform->Update();

				if (!Platform->IsMainWindowMinimized() && Platform->HasActiveMainWindow())
				{
					// Update game state
					PositionX += GGlobalTimer->Delta();
					auto& Param = MvpBuffer->GetParamForWriting<matrix4x4>("mWorldViewProj");
					
					float Width = (float)MainWindow->GetWidth();
					float Height = (float)MainWindow->GetHeight();
					Width = Width > 0 ? Width: 1;
					Height = Height > 0 ? Height : 1;
					auto Projection = glm::perspectiveFovLH_ZO(glm::radians(45.0f), (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.1f, 100.0f);
					auto View = glm::lookAtLH(float3(0.0f, 2.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));
					// Param = Param * glm::translate(glm::identity<matrix4x4>(), float3(0.0f, glm::sin(PositionX), 0.0f));
					Param = Projection * View;
					Param = transpose(Param);

					// Render the frame
					TRenderThread::Submit(
						[&, this]
						{
							static u32 FrameIndex = 0;

							auto pImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
							auto SwapChain = LowLevelRenderer->GetSwapChain(0);

							// Transfer
							MvpBuffer->RT_UploadToGPU(pImmList);

							// Render
							if (SwapChain)
							{
								pImmList->BeginDebugEvent("Clear Render State");
								pImmList->ClearSamplers();
								pImmList->EndDebugEvent();

								pImmList->BeginDebugEvent("Mesh Pass");
								pImmList->BindParamBuffers(MvpBuffer, 0);
								pImmList->BindVertexBuffers(VertexBuffer, 0, 0);
								pImmList->BindIndexBuffer(IndexBuffer, 0);

								pImmList->SetViewport(0, 0, (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.0f, 1.0f);
								pImmList->SetScissor(0, 0,  (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight());
							
								// Write to quad render target
								pImmList->StartDrawingToRenderTargets(RenderTargets[FrameIndex], DepthTarget);
								pImmList->ClearDepthTarget(DepthTarget);
								pImmList->ClearRenderTarget(RenderTargets[FrameIndex], {0.1f, 0.1f, 0.1f, 1.0f});
								pImmList->BindPipeline(UnlitPipeline);
								pImmList->BindSamplers(Samplers);
								pImmList->DrawIndexed(IndexBuffer->GetCount(), 0, 0);
								pImmList->EndDebugEvent();

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

}