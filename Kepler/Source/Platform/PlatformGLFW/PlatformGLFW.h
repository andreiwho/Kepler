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

namespace Kepler
{
	class TPlatformGLFW : public TPlatform
	{
	public:
		TPlatformGLFW();
		~TPlatformGLFW();

		virtual TWindow* CreatePlatformWindow(i32 Width, i32 Height, const std::string& Title, const TWindowParams& Params = {}) override;
		virtual void Update() override;
		virtual bool HasActiveMainWindow() const override;
		virtual void OnPlatformEvent(const TPlatformEventBase& Event) override;
		static bool HandleCrashReported_Impl(const std::string& Message);
		inline virtual bool IsMainWindow(TWindow* Window) const override { return !Windows.IsEmpty() && (Windows[0].get() == Window); }

	private:
		void DestroyClosedWindows();
		void Terminate();

	protected:
		virtual void CloseAllWindows() override;

	private:
		TDynArray<std::unique_ptr<TWindow>> Windows;
	};
}
#endif