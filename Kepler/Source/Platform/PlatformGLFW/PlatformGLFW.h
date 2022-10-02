#pragma once
#ifdef PLATFORM_DESKTOP
#include "Platform/Platform.h"
#include "Core/Types.h"
#include "Core/Containers/DynArray.h"

#include <memory>
#include <vector>

#ifdef CreateWindow
#  undef CreateWindow
#endif

namespace ke
{
	class TPlatformGLFW : public TPlatform
	{
	public:
		TPlatformGLFW();
		~TPlatformGLFW();

		virtual TWindow* CreatePlatformWindow(i32 width, i32 height, const TString& title, const TWindowParams& params = {}) override;
		virtual void Update() override;
		virtual bool HasActiveMainWindow() const override;
		virtual void OnPlatformEvent(const TPlatformEventBase& event) override;
		static bool HandleCrashReported_Impl(const TString& msg);
		inline virtual bool IsMainWindow(TWindow* pWindow) const override { return !m_Windows.IsEmpty() && (m_Windows[0].get() == pWindow); }
		virtual void SetCursorMode(ECursorMode mode) override;

	private:
		void DestroyClosedWindows();
		void Terminate();

	protected:
		virtual void CloseAllWindows() override;

	private:
		TDynArray<std::unique_ptr<TWindow>> m_Windows;
	};
}
#endif