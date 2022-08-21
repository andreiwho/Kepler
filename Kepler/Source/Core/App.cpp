#include "App.h"
#include "Log.h"

#include "Platform/Platform.h"

#include <iostream>
#include "Timer.h"
#include "Async/Async.h"
#include "Math/Vector.h"

// Test
#include "Renderer/HLSLShaderCompiler.h"
#include "Core/Filesystem/FileUtils.h"
#include "Renderer/Elements/IndexBuffer.h"
#include "Renderer/Elements/HLSLShader.h"

namespace Kepler
{
	TCommandLineArguments::TCommandLineArguments(TDynArray<std::string> const& CommandLine)
	{
		// Parse command line args
		for (const std::string& arg : CommandLine)
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

		// Begin main loop
		TTimer MainTimer{};
		GGlobalTimer = &MainTimer;
		float DisplayInfoTime = 0.0f;

		struct TVertex
		{
			float3 Pos{};
			float4 Col{};
		};

		TDynArray<TVertex> Vertices = {
			{{0.0f, 0.1f, 0.0f}, {0.0f, 1.0, 0.0f, 1.0f}},
			{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}
		};
		TRef<TDataBlob> Blob = TDataBlob::CreateGraphicsDataBlob(Vertices);


		TRef<TVertexBuffer> VertexBuffer = Await(TRenderThread::Submit(
			[Blob, this]
			{
				return LowLevelRenderer->GetRenderDevice()->CreateVertexBuffer(EBufferAccessFlags::GPUOnly, Blob);
			}));

		TRef<TVertexBuffer> VertexBuffer1 = Await(TRenderThread::Submit(
			[Blob, this]
			{
				return LowLevelRenderer->GetRenderDevice()->CreateVertexBuffer(EBufferAccessFlags::GPUOnly, Blob);
			}));

		TDynArray<u32> Indices = { 0,1,2 };
		TRef<TIndexBuffer> IndexBuffer = Await(TRenderThread::Submit(
			[this, &Indices] 
			{
				return LowLevelRenderer->GetRenderDevice()->CreateIndexBuffer(EBufferAccessFlags::GPUOnly, TDataBlob::CreateGraphicsDataBlob(Indices));
			}));


		auto Shader = Await(TRenderThread::Submit(
			[&]
			{
				TRef<THLSLShaderCompiler> Compiler = THLSLShaderCompiler::CreateShaderCompiler();
				return Compiler->CompileShader("Kepler/Shaders/Source/Core/DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
			}
		));

		constexpr float3 Vec(7.0f, 1.0f, 0.0f);
		constexpr float4 Vec1(0.0f, 0.0f, 0.0f, 1.0f);
		float3 Result = Normalize(Vec * Vec1 * 15.0f);

		const std::string InitialWindowName = MainWindow->GetTitle();
		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			while (Platform->HasActiveMainWindow())
			{
				MainTimer.Begin();
				Platform->Update();

				if (!Platform->IsMainWindowMinimized())
				{
					// Update game state

					// Render the frame
					TRenderThread::Submit(
						[this, VertexBuffer1, VertexBuffer, IndexBuffer, Shader]
						{
							auto pImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
							auto SwapChain = LowLevelRenderer->GetSwapChain(0);

							if (SwapChain)
							{
								pImmList->BindVertexBuffers({ VertexBuffer, VertexBuffer1 }, 0, { 0, 0 });
								pImmList->BindIndexBuffer(IndexBuffer, 0);

								pImmList->StartDrawingToSwapChainImage(SwapChain.Raw());
								float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
								pImmList->ClearSwapChainImage(SwapChain.Raw(), ClearColor);
								pImmList->BindShader(Shader);

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
						MainWindow->SetTitle(fmt::format("{} <{}>", InitialWindowName, 1.0f / MainTimer.Delta()));
					}
				}
#endif
			}
		}
	}

	void TApplication::OnPlatformEvent(const TPlatformEventBase& Event)
	{
		TPlatformEventDispatcher Dispatcher{ Event };
		Dispatcher.Dispatch(this, &TApplication::OnWindowClosed);
		Dispatcher.Dispatch(this, &TApplication::OnWindowResized);
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