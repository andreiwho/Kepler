#include "PlatformEvent.h"
#include "Window.h"

#include <string>

namespace ke
{

	TString EPlatformEventType::ToString() const
	{
		switch (Value)
		{
		case ke::EPlatformEventType::MouseMove:
			return "MouseMove";
		case ke::EPlatformEventType::MouseButtonDown:
			return "MouseButtonDown";
		case ke::EPlatformEventType::MouseButtonUp:
			return "MouseButtonUp";
		case ke::EPlatformEventType::MouseScrollWheel:
			return "MouseScrollWheel";
		case ke::EPlatformEventType::KeyDown:
			return "KeyDown";
		case ke::EPlatformEventType::KeyUp:
			return "KeyUp";
		case ke::EPlatformEventType::KeyHold:
			return "KeyHold";
		case ke::EPlatformEventType::Char:
			return "Char";
		case ke::EPlatformEventType::WindowMove:
			return "WindowMove";
		case ke::EPlatformEventType::WindowSize:
			return "WindowSize";
		case ke::EPlatformEventType::WindowMaximize:
			return "WindowMaximize";
		case ke::EPlatformEventType::WindowMinimize:
			return "WindowMinimize";
		case ke::EPlatformEventType::WindowRestore:
			return "WindowRestore";
		case ke::EPlatformEventType::MouseEnter:
			return "MouseEnter";
		case ke::EPlatformEventType::MouseLeave:
			return "MouseLeave";
		}
		return "Unknown";
	}

	TString EPlatformEventCategory::ToString() const
	{
		switch (Value)
		{
		case ke::EPlatformEventCategory::Input:
			return "Input";
		case ke::EPlatformEventCategory::Mouse:
			return "Mouse | Input";
		case ke::EPlatformEventCategory::Keyboard:
			return "Keyboard | Input";
		case ke::EPlatformEventCategory::Window:
			return "Window";
		case ke::EPlatformEventCategory::Other:
			return "Other";
		}
		return "Unknown";
	}

	TPlatformEventBase::TPlatformEventBase(class TWindow* window, EPlatformEventType type, EPlatformEventCategory category)
		:	TypeMask(type.Value | category.Value), Window(window)
	{
	}

	bool TPlatformEventBase::IsOfType(EPlatformEventType type) const
	{
		return TypeMask & type;
	}

	bool TPlatformEventBase::IsOfCategory(EPlatformEventCategory category) const
	{
		return TypeMask & category;
	}
}