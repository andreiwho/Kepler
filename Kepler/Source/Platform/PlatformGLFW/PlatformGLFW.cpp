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

namespace ke
{
	TPlatformGLFW::TPlatformGLFW()
	{
		m_bInitialized = !!glfwInit();
		KEPLER_INFO(LogPlatform, "GLFW platform initialized");
		if (!m_bInitialized)
		{
			// Throw some kind of an error
		}

		// Specify that we will not be using OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	TPlatformGLFW::~TPlatformGLFW()
	{
		if (m_bInitialized)
		{
			glfwTerminate();
			KEPLER_INFO(LogPlatform, "GLFW platform terminated");
		}
	}

	TWindow* TPlatformGLFW::CreatePlatformWindow(i32 width, i32 height, const TString& title, const TWindowParams& params)
	{
		KEPLER_INFO(LogPlatform, "Creating GLFW platform window '{}'", title);
		return m_Windows.EmplaceBack(std::make_unique<TWindowGLFW>(width, height, title, params)).get();
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
		return !m_Windows.IsEmpty() && !!m_Windows[0];
	}

	void TPlatformGLFW::OnPlatformEvent(const TPlatformEventBase& event)
	{
		// Handle the event if needed
		// ...
		// After handling pass this one to the GenericPlatformInterface
		TPlatform::OnPlatformEvent(event);
	}

	bool TPlatformGLFW::HandleCrashReported_Impl(const TString& msg)
	{
		if (::MessageBoxA(nullptr, msg.c_str(), "Crash Reported", MB_OK | MB_ICONERROR) == IDOK)
		{
			return true;
		}
		return false;
	}

	void TPlatformGLFW::SetCursorMode(ECursorMode mode)
	{
		TPlatform::SetCursorMode(mode);
		if (m_OldCursorMode == m_CurrentCursorMode)
		{
			return;
		}

		if (!HasActiveMainWindow())
		{
			return;
		}

		TWindowGLFW* pWindow = (TWindowGLFW*)m_Windows[0].get();
		if (!pWindow)
		{
			return;
		}

		switch (mode)
		{
		case ECursorMode::Visible:
			glfwSetInputMode(pWindow->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case ECursorMode::HiddenFree:
			glfwSetInputMode(pWindow->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
		case ECursorMode::HiddenLocked:
			glfwSetInputMode(pWindow->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetInputMode(pWindow->GetGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			break;
		default:
			break;
		}
	}

	void TPlatformGLFW::DestroyClosedWindows()
	{
		for (auto& window : m_Windows)
		{
			if (window && window->IsCloseRequested())
			{
				window.reset();
			}
		}
	}

	void TPlatformGLFW::Terminate()
	{
		for (auto& window : m_Windows)
		{
			window.reset();
		}
	}

	void TPlatformGLFW::CloseAllWindows()
	{
		m_Windows.Clear();
	}

	TSharedPtr<TPlatform> TPlatform::CreatePlatformInterface()
	{
		return MakeShared<TPlatformGLFW>();
	}
}

#endif