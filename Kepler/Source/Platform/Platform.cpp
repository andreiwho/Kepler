#include "Platform.h"

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

	void TPlatform::OnPlatformEvent(const TPlatformEventBase& event)
	{
		TPlatformEventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseMoved);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseButtonReleased);
		dispatcher.Dispatch(this, &TPlatform::Internal_MouseButtonPressed);
		dispatcher.Dispatch(this, &TPlatform::Internal_KeyPressed);
		dispatcher.Dispatch(this, &TPlatform::Internal_KeyReleased);

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

}