#include "PlatformGLFW.h"
#ifdef PLATFORM_DESKTOP
#include "Core/Log.h"
#include "WindowGLFW.h"

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#else

#endif

#include <cassert>

#include <GLFW/glfw3.h>

#ifdef CreateWindow
#  undef CreateWindow
#endif

namespace Kepler
{
	TPlatformGLFW::TPlatformGLFW()
	{
		bInitialized = !!glfwInit();
		KEPLER_INFO(LogPlatform, "GLFW platform initialized");
		if (!bInitialized)
		{
			// Throw some kind of an error
		}

		// Specify that we will not be using OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	TPlatformGLFW::~TPlatformGLFW()
	{
		if (bInitialized)
		{
			glfwTerminate();
			KEPLER_INFO(LogPlatform, "GLFW platform terminated");
		}
	}

	TWindow* TPlatformGLFW::CreatePlatformWindow(i32 Width, i32 Height, const TString& Title, const TWindowParams& Params)
	{
		KEPLER_INFO(LogPlatform, "Creating GLFW platform window '{}'", Title);
		return Windows.EmplaceBack(std::make_unique<TWindowGLFW>(Width, Height, Title, Params)).get();
	}

	void TPlatformGLFW::Update()
	{
		TPlatform::Update();

		glfwPollEvents();
		DestroyClosedWindows();

		if (!HasActiveMainWindow())
		{
			Terminate();
			return;
		}
	}

	bool TPlatformGLFW::HasActiveMainWindow() const
	{
		return !Windows.IsEmpty() && !!Windows[0];
	}

	void TPlatformGLFW::OnPlatformEvent(const TPlatformEventBase& event)
	{
		// Handle the event if needed
		// ...
		// After handling pass this one to the GenericPlatformInterface
		TPlatform::OnPlatformEvent(event);
	}

	bool TPlatformGLFW::HandleCrashReported_Impl(const TString& Message)
	{
		if (::MessageBoxA(nullptr, Message.c_str(), "Crash Reported", MB_OK | MB_ICONERROR) == IDOK)
		{
			return true;
		}
		return false;
	}

	void TPlatformGLFW::SetCursorMode(ECursorMode Mode)
	{
		TPlatform::SetCursorMode(Mode);
		if (OldCursorMode == CurrentCursorMode)
		{
			return;
		}

		if (!HasActiveMainWindow())
		{
			return;
		}

		TWindowGLFW* Window = (TWindowGLFW*)Windows[0].get();
		if (!Window)
		{
			return;
		}

		switch (Mode)
		{
		case ECursorMode::Visible:
			glfwSetInputMode(Window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case ECursorMode::HiddenFree:
			glfwSetInputMode(Window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
		case ECursorMode::HiddenLocked:
			glfwSetInputMode(Window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetInputMode(Window->GetGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			break;
		default:
			break;
		}
	}

	void TPlatformGLFW::DestroyClosedWindows()
	{
		for (auto& window : Windows)
		{
			if (window && window->IsCloseRequested())
			{
				window.reset();
			}
		}
	}

	void TPlatformGLFW::Terminate()
	{
		for (auto& window : Windows)
		{
			window.reset();
		}
	}

	void TPlatformGLFW::CloseAllWindows()
	{
		Windows.Clear();
	}

	TSharedPtr<TPlatform> TPlatform::CreatePlatformInterface()
	{
		return MakeShared<TPlatformGLFW>();
	}
}

#endif