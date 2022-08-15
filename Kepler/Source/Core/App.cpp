#include "App.h"
#include "Log.h"

#include "Platform/Platform.h"

#include <iostream>
#include "Timer.h"
#include "Async/Async.h"

// Test
#include "Renderer/ShaderCompiler.h"
#include "Core/Filesystem/FileUtils.h"

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
		KEPLER_INFO("LogApp", "Starting application initialization");

		MainWindow = CHECKED(TPlatform::Get()->CreatePlatformWindow(1280, 720, "Kepler"));
		LowLevelRenderer = MakeShared<TLowLevelRenderer>();
		LowLevelRenderer->InitRenderStateForWindow(MainWindow);
	}

	TApplication::~TApplication()
	{
		LowLevelRenderer.reset();
		KEPLER_INFO("LogApp", "Finishing application termination");
	}

	void TApplication::Run()
	{
		KEPLER_INFO("LogApp", "Application Run called...");

		// Begin main loop
		TTimer MainTimer{};
		GGlobalTimer = &MainTimer;
		float DisplayInfoTime = 0.0f;

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
						[this]
						{
							auto pImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
							auto SwapChain = LowLevelRenderer->GetSwapChain(0);

							if (SwapChain)
							{
								pImmList->StartDrawingToSwapChainImage(SwapChain.Raw());
								float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
								pImmList->ClearSwapChainImage(SwapChain.Raw(), ClearColor);
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