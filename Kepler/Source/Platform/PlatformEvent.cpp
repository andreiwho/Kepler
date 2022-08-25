#include "PlatformEvent.h"
#include "Window.h"

#include <string>

namespace Kepler
{

	TString EPlatformEventType::ToString() const
	{
		switch (Value)
		{
		case Kepler::EPlatformEventType::MouseMove:
			return "MouseMove";
		case Kepler::EPlatformEventType::MouseButtonDown:
			return "MouseButtonDown";
		case Kepler::EPlatformEventType::MouseButtonUp:
			return "MouseButtonUp";
		case Kepler::EPlatformEventType::MouseScrollWheel:
			return "MouseScrollWheel";
		case Kepler::EPlatformEventType::KeyDown:
			return "KeyDown";
		case Kepler::EPlatformEventType::KeyUp:
			return "KeyUp";
		case Kepler::EPlatformEventType::KeyHold:
			return "KeyHold";
		case Kepler::EPlatformEventType::Char:
			return "Char";
		case Kepler::EPlatformEventType::WindowMove:
			return "WindowMove";
		case Kepler::EPlatformEventType::WindowSize:
			return "WindowSize";
		case Kepler::EPlatformEventType::WindowMaximize:
			return "WindowMaximize";
		case Kepler::EPlatformEventType::WindowMinimize:
			return "WindowMinimize";
		case Kepler::EPlatformEventType::WindowRestore:
			return "WindowRestore";
		case Kepler::EPlatformEventType::MouseEnter:
			return "MouseEnter";
		case Kepler::EPlatformEventType::MouseLeave:
			return "MouseLeave";
		}
		return "Unknown";
	}

	TString EPlatformEventCategory::ToString() const
	{
		switch (Value)
		{
		case Kepler::EPlatformEventCategory::Input:
			return "Input";
		case Kepler::EPlatformEventCategory::Mouse:
			return "Mouse | Input";
		case Kepler::EPlatformEventCategory::Keyboard:
			return "Keyboard | Input";
		case Kepler::EPlatformEventCategory::Window:
			return "Window";
		case Kepler::EPlatformEventCategory::Other:
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