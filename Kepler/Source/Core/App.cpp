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
	}

	TApplication::~TApplication()
	{
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
			float4 Col{};
		};

		TDynArray<TVertex> Vertices = {
			{{-0.5f, 0.0f,  0.5f }, {0.0f, 1.0f, 0.0f, 1.0f}},
			{{ 0.5f, 0.0f,  0.5f }, {0.0f, 1.0f, 0.0f, 1.0f}},
			{{ 0.5f, 0.0f, -0.5f  }, {1.0f, 0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.0f, -0.5f  }, {1.0f, 0.0f, 0.0f, 1.0f}},
		};
		TDynArray<u32> Indices = { 0,1,3,1,2,3 };

		TRef<TPipelineParamMapping> Mapping = MakeRef(New<TPipelineParamMapping>());
		Mapping->AddParam("mWorldViewProj", 0, 0, EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);

		TRef<TParamBuffer> MvpBuffer;
		TRef<TImage2D> Image;
		TRef<TVertexBuffer> VertexBuffer;
		TRef<TIndexBuffer> IndexBuffer;
		TRef<TGraphicsPipeline> Pipeline;

		TRef<TDataBlob> VertexBlob = TDataBlob::New(Vertices);
		auto RenderTask = TRenderThread::Submit(
			[&, this]
			{
				MvpBuffer = TParamBuffer::New(Mapping);
				Image = TImage2D::New(1280, 720, EFormat::R8G8B8A8_UNORM, EImageUsage::ShaderResource);
				VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, VertexBlob);
				IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Indices));
				TRef<TTransferBuffer> Transfer = TTransferBuffer::New(VertexBlob->GetSize(), VertexBlob);
				Transfer->Transfer(LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList(), VertexBuffer, 0, 0, VertexBlob->GetSize());
				Pipeline = MakeRef(New<TDefaultUnlitPipeline>());
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

				if (!Platform->IsMainWindowMinimized())
				{
					// Update game state
					PositionX += GGlobalTimer->Delta();
					auto& Param = *MvpBuffer->GetParam<matrix4x4>("mWorldViewProj");
					
					auto Projection = glm::perspectiveFovLH_ZO(glm::radians(45.0f), (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.1f, 100.0f);
					auto View = glm::lookAtLH(float3(0.0f, 2.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));
					// Param = Param * glm::translate(glm::identity<matrix4x4>(), float3(0.0f, glm::sin(PositionX), 0.0f));
					Param = Projection * View;
					Param = transpose(Param);

					// Render the frame
					TRenderThread::Submit(
						[this, VertexBuffer, IndexBuffer, Pipeline, MvpBuffer]
						{
							auto pImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
							auto SwapChain = LowLevelRenderer->GetSwapChain(0);

							// Transfer
							MvpBuffer->RT_UploadToGPU(pImmList);

							// Render
							if (SwapChain)
							{
								pImmList->BindParamBuffers(MvpBuffer, 0);
								pImmList->BindVertexBuffers(VertexBuffer, 0, 0);
								pImmList->BindIndexBuffer(IndexBuffer, 0);

								pImmList->StartDrawingToSwapChainImage(SwapChain.Raw());
								pImmList->SetViewport(0, 0, (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight(), 0.0f, 1.0f);
								pImmList->SetScissor(0, 0,  (float)MainWindow->GetWidth(), (float)MainWindow->GetHeight());
								float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
								pImmList->ClearSwapChainImage(SwapChain.Raw(), ClearColor);
								pImmList->BindPipeline(Pipeline);

								pImmList->DrawIndexed(IndexBuffer->GetCount(), 0, 0);
							}
						});

					LowLevelRenderer->PresentAll();
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