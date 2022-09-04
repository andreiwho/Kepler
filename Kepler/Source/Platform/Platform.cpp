#include "Platform.h"

#ifdef PLATFORM_DESKTOP
# include "Platform/PlatformGLFW/PlatformGLFW.h"
#else

#endif
#include "Core/Log.h"

namespace Kepler
{
	TPlatform* TPlatform::Instance = nullptr;

	TPlatform::TPlatform()
	{
		Instance = this;
	}

	TPlatform::~TPlatform()
	{
		if (bInitialized)
		{
			// Do something
			bInitialized = false;
		}
	}

	TPlatform* TPlatform::Get()
	{
		return Instance;
	}

	bool TPlatform::HandleCrashReported(const TString& Message)
	{
#ifdef PLATFORM_DESKTOP
		static TAtomic<bool> bFired = false;
		if (!bFired)
		{
			bFired.store(true);
			return TPlatformGLFW::HandleCrashReported_Impl(Message);
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

		if (EventListener)
		{
			EventListener->OnPlatformEvent(event);
		}
	}

	void TPlatform::RegisterPlatformEventListener(IPlatformEventListener* listener)
	{
		EventListener = listener;
	}

	bool TPlatform::Internal_MouseMoved(const TMouseMoveEvent& e)
	{
		MouseState.OnMouseMove({ e.X, e.Y });
		return false;
	}

	bool TPlatform::Internal_MouseButtonPressed(const TMouseButtonDownEvent& e)
	{
		MouseState.OnButtonPressed(e.Button);
		return false;
	}

	bool TPlatform::Internal_MouseButtonReleased(const TMouseButtonUpEvent& e)
	{
		MouseState.OnButtonReleased(e.Button);
		return false;
	}

	bool TPlatform::Internal_KeyPressed(const TKeyDownEvent& e)
	{
		KeyboardState.OnKeyPressed(e.Key);
		return false;
	}

	bool TPlatform::Internal_KeyReleased(const TKeyDownEvent& e)
	{
		KeyboardState.OnKeyReleased(e.Key);
		return false;
	}

	bool TPlatform::Internal_WindowClosed(const TWindowClosedEvent& Event)
	{
		KEPLER_TRACE(LogPlatform, "Window {} closed", Event.Window->GetName());
		if (IsMainWindow(Event.Window))
		{
			CloseAllWindows();
		}

		return false;
	}

	bool TPlatform::Internal_WindowMinimized(const TWindowMinimizeEvent& Event)
	{
		if (IsMainWindow(Event.Window))
		{
			bMinimized = true;
		}
		return false;
	}

	bool TPlatform::Internal_WindowRestored(const TWindowRestoreEvent& Event)
	{
		if (IsMainWindow(Event.Window) && bMinimized)
		{
			bMinimized = false;
		}
		return false;
	}

	bool TPlatform::IsMainWindowMinimized() const
	{
		return bMinimized;
	}

}