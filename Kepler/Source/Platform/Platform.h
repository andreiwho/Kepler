#pragma once
#include "Core/Core.h"
#include "PlatformEvent.h"
#include "Window.h"
#include "Core/Malloc.h"

#include <memory>

#ifdef CreateWindow
#  undef CreateWindow
#endif

DEFINE_UNIQUE_LOG_CHANNEL(LogPlatform);

namespace ke
{
	enum class ECursorMode
	{
		Visible,
		HiddenFree,
		HiddenLocked,
	};

	struct TPlatform
	{
		TPlatform();
		virtual ~TPlatform();

		static TPlatform* Get();

		static TSharedPtr<TPlatform> CreatePlatformInterface();

		static bool HandleCrashReported(const TString& Message);

		virtual TWindow* CreatePlatformWindow(i32 width, i32 height, const TString& title, const TWindowParams& params = {}) = 0;
		virtual void Update() { MouseState.OnUpdate(); };
		virtual bool HasActiveMainWindow() const = 0;
		virtual void OnPlatformEvent(const TPlatformEventBase& event);
		void RegisterPlatformEventListener(IPlatformEventListener* listener);

		inline const TKeyboardState& GetKeyboardState() const { return KeyboardState; }
		inline const TMouseState& GetMouseState() const { return MouseState; }
		virtual bool IsMainWindow(TWindow* Window) const { return true; }

		inline ECursorMode GetCurrentCursorMode() const { return CurrentCursorMode; }
		virtual void SetCursorMode(ECursorMode Mode);

	private:
		bool Internal_MouseMoved(const TMouseMoveEvent& e);
		bool Internal_MouseButtonPressed(const TMouseButtonDownEvent& e);
		bool Internal_MouseButtonReleased(const TMouseButtonUpEvent& e);
		bool Internal_KeyPressed(const TKeyDownEvent& e);
		bool Internal_KeyReleased(const TKeyUpEvent& e);
		bool Internal_WindowClosed(const TWindowClosedEvent& Event);
		bool Internal_WindowMinimized(const TWindowMinimizeEvent& Event);
		bool Internal_WindowRestored(const TWindowRestoreEvent& Event);
		bool Internal_WindowFocused(const TWindowFocusedEvent& Event);
		bool Internal_WindowUnfocused(const TWindowUnfocusedEvent& Event);

	protected:
		bool bInitialized = false;
		virtual void CloseAllWindows() = 0;
		ECursorMode CurrentCursorMode{ECursorMode::Visible};
		ECursorMode OldCursorMode{ECursorMode::Visible};

	private:
		static TPlatform* Instance;

		IPlatformEventListener* EventListener{ nullptr };
		TMouseState MouseState{};
		TKeyboardState KeyboardState{};

	public:
		bool bMinimized = false;
		bool bUnfocused = false;
		bool IsMainWindowMinimized() const;
		inline bool IsMainWindowUnfocused() const
		{
			return bUnfocused;
		}
	};
}