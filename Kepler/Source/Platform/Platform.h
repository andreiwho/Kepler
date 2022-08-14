#pragma once
#include "Core/Types.h"
#include "PlatformEvent.h"
#include "Window.h"
#include "Core/Malloc.h"

#include <memory>

#ifdef CreateWindow
#  undef CreateWindow
#endif

namespace Kepler
{
	struct TPlatform
	{
		TPlatform();
		virtual ~TPlatform();

		static TPlatform* Get();

		static TRef<TPlatform> CreatePlatformInterface();

		static bool HandleCrashReported(const std::string& Message);

		virtual TWindow* CreatePlatformWindow(i32 width, i32 height, const std::string& title, const TWindowParams& params = {}) = 0;
		virtual void Update() = 0;
		virtual bool HasActiveMainWindow() const = 0;
		virtual void OnPlatformEvent(const TPlatformEventBase& event);
		void RegisterPlatformEventListener(IPlatformEventListener* listener);

		inline const TKeyboardState& GetKeyboardState() const { return KeyboardState; }
		inline const TMouseState& GetMouseState() const { return MouseState; }

	private:
		bool Internal_MouseMoved(const TMouseMoveEvent& e);
		bool Internal_MouseButtonPressed(const TMouseButtonDownEvent& e);
		bool Internal_MouseButtonReleased(const TMouseButtonUpEvent& e);
		bool Internal_KeyPressed(const TKeyDownEvent& e);
		bool Internal_KeyReleased(const TKeyDownEvent& e);
		bool Internal_WindowClosed(const TWindowClosedEvent& Event);

	protected:
		bool bInitialized = false;

	private:
		static TPlatform* Instance;

		IPlatformEventListener* EventListener{ nullptr };
		TMouseState MouseState{};
		TKeyboardState KeyboardState{};
	};
}