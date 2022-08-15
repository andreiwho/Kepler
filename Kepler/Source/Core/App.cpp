#include "App.h"
#include "Log.h"

#include "Platform/Platform.h"

#include <iostream>

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
		LowLevelRenderer->InitRenderStateForWindow(TPlatform::Get()->CreatePlatformWindow(640, 640, "Other"));
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
		if (TPlatform* Platform = TPlatform::Get())
		{
			Platform->RegisterPlatformEventListener(this);
			while (Platform->HasActiveMainWindow())
			{
				Platform->Update();

				TRenderThread::Submit(
					[this]
					{
						auto ImmList = LowLevelRenderer->GetRenderDevice()->GetImmediateCommandList();
						auto SwapChain = LowLevelRenderer->GetSwapChain(0);

						if (SwapChain)
						{
							ImmList->StartDrawingToSwapChainImage(SwapChain.Raw());
							float ClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
							ImmList->ClearSwapChainImage(SwapChain.Raw(), ClearColor);
						}

						SwapChain = LowLevelRenderer->GetSwapChain(1);
						if (SwapChain)
						{
							ImmList->StartDrawingToSwapChainImage(SwapChain.Raw());
							float ClearColor1[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
							ImmList->ClearSwapChainImage(SwapChain.Raw(), ClearColor1);
						}
					});

				LowLevelRenderer->PresentAll();
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