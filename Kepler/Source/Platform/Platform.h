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
		virtual void Update() { m_MouseState.OnUpdate(); };
		virtual bool HasActiveMainWindow() const = 0;
		virtual void OnPlatformEvent(const TPlatformEventBase& event);
		void RegisterPlatformEventListener(IPlatformEventListener* listener);

		inline const TKeyboardState& GetKeyboardState() const { return m_KeyboardState; }
		inline const TMouseState& GetMouseState() const { return m_MouseState; }
		virtual bool IsMainWindow(TWindow* pWindow) const { return true; }

		inline ECursorMode GetCurrentCursorMode() const { return m_CurrentCursorMode; }
		virtual void SetCursorMode(ECursorMode mode);

	private:
		bool Internal_MouseMoved(const TMouseMoveEvent& e);
		bool Internal_MouseButtonPressed(const TMouseButtonDownEvent& e);
		bool Internal_MouseButtonReleased(const TMouseButtonUpEvent& e);
		bool Internal_KeyPressed(const TKeyDownEvent& e);
		bool Internal_KeyReleased(const TKeyUpEvent& e);
		bool Internal_WindowClosed(const TWindowClosedEvent& e);
		bool Internal_WindowMinimized(const TWindowMinimizeEvent& e);
		bool Internal_WindowRestored(const TWindowRestoreEvent& e);
		bool Internal_WindowFocused(const TWindowFocusedEvent& e);
		bool Internal_WindowUnfocused(const TWindowUnfocusedEvent& e);

	protected:
		bool m_bInitialized = false;
		virtual void CloseAllWindows() = 0;
		ECursorMode m_CurrentCursorMode{ECursorMode::Visible};
		ECursorMode m_OldCursorMode{ECursorMode::Visible};

	private:
		static TPlatform* Instance;

		IPlatformEventListener* m_EventListener{ nullptr };
		TMouseState m_MouseState{};
		TKeyboardState m_KeyboardState{};

	public:
		bool m_bMinimized = false;
		bool m_bUnfocused = false;
		bool IsMainWindowMinimized() const;
		inline bool IsMainWindowUnfocused() const
		{
			return m_bUnfocused;
		}
	};
}