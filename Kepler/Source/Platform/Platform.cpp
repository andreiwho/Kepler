#include "Platform.h"

#ifdef PLATFORM_DESKTOP
# include "Platform/PlatformGLFW/PlatformGLFW.h"
#else

#endif
#include "Core/Log.h"

namespace ke
{
	TPlatform* TPlatform::Instance = nullptr;

	TPlatform::TPlatform()
	{
		Instance = this;
	}

	TPlatform::~TPlatform()
	{
		if (m_bInitialized)
		{
			// Do something
			m_bInitialized = false;
		}
	}

	TPlatform* TPlatform::Get()
	{
		return Instance;
	}

	bool TPlatform::HandleCrashReported(const String& msg)
	{
#ifdef PLATFORM_DESKTOP
		static TAtomic<bool> bFired = false;
		if (!bFired)
		{
			bFired.store(true);
			return TPlatformGLFW::HandleCrashReported_Impl(msg);
		}
#else
		return true;
#endif
		return false;
	}

	void TPlatform::OnPlatformEvent(const TPlatformEventBase& event)
	{
		TPlatformEventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseMoved);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseButtonReleased);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseButtonPressed);
		dispatcher.Dispatch(this, &TPlatform::Internal_KeyPressed);
		dispatcher.Dispatch(this, &TPlatform::Internal_KeyReleased);
		dispatcher.Dispatch(this, &TPlatform::Internal_WindowClosed);
		dispatcher.Dispatch(this, &TPlatform::Internal_WindowMinimized);
		dispatcher.Dispatch(this, &TPlatform::Internal_WindowRestored);
		dispatcher.Dispatch(this, &TPlatform::Internal_WindowFocused);
		dispatcher.Dispatch(this, &TPlatform::Internal_WindowUnfocused);

		if (m_EventListener)
		{
			m_EventListener->OnPlatformEvent(event);
		}
	}

	void TPlatform::RegisterPlatformEventListener(IPlatformEventListener* listener)
	{
		m_EventListener = listener;
	}

	void TPlatform::SetCursorMode(ECursorMode mode)
	{
		m_OldCursorMode = m_CurrentCursorMode;
		m_CurrentCursorMode = mode;
	}

	bool TPlatform::Internal_MouseMoved(const TMouseMoveEvent& e)
	{
		m_MouseState.OnMouseMove({ e.X, e.Y });
		return false;
	}

	bool TPlatform::Internal_MouseButtonPressed(const TMouseButtonDownEvent& e)
	{
		m_MouseState.OnButtonPressed(e.Button);
		return false;
	}

	bool TPlatform::Internal_MouseButtonReleased(const TMouseButtonUpEvent& e)
	{
		m_MouseState.OnButtonReleased(e.Button);
		return false;
	}

	bool TPlatform::Internal_KeyPressed(const TKeyDownEvent& e)
	{
		m_KeyboardState.OnKeyPressed(e.Key);
		return false;
	}

	bool TPlatform::Internal_KeyReleased(const TKeyUpEvent& e)
	{
		m_KeyboardState.OnKeyReleased(e.Key);
		return false;
	}

	bool TPlatform::Internal_WindowClosed(const TWindowClosedEvent& e)
	{
		KEPLER_TRACE(LogPlatform, "Window {} closed", e.Window->GetName());
		if (IsMainWindow(e.Window))
		{
			CloseAllWindows();
		}

		return false;
	}

	bool TPlatform::Internal_WindowMinimized(const TWindowMinimizeEvent& e)
	{
		if (IsMainWindow(e.Window))
		{
			m_bMinimized = true;
		}
		return false;
	}

	bool TPlatform::Internal_WindowRestored(const TWindowRestoreEvent& e)
	{
		if (IsMainWindow(e.Window) && m_bMinimized)
		{
			m_bMinimized = false;
		}
		return false;
	}

	bool TPlatform::Internal_WindowFocused(const TWindowFocusedEvent& e)
	{
		if (IsMainWindow(e.Window) && m_bUnfocused)
		{
			m_bUnfocused = false;
		}
		return false;
	}

	bool TPlatform::Internal_WindowUnfocused(const TWindowUnfocusedEvent& e)
	{
		if (IsMainWindow(e.Window))
		{
			m_bUnfocused = true;
		}
		return false;
	}

	bool TPlatform::IsMainWindowMinimized() const
	{
		return m_bMinimized;
	}

}