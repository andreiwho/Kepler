#pragma once
#ifdef PLATFORM_DESKTOP
#include "Platform/Window.h"

#include <GLFW/glfw3.h>

namespace Kepler
{
	class TWindowGLFW : public TWindow
	{
	public:
		TWindowGLFW(i32 Width, i32 Height, const TString& Title, const TWindowParams& Params = {});
		~TWindowGLFW();

		virtual void* GetNativeHandle() const override;
		void RequestClose();
		inline GLFWwindow* GetGLFWWindow() const { return Window; }
		void Internal_UpdateSize(i32 width, i32 height);
	private:
		void SetupCallbacks();

	protected:
		virtual void SetTitle_Impl(const TString& newTitle) override;

		virtual void SetMaximized_Impl(bool bNewMaximized) override;

		virtual void SetDecorated_Impl(bool bNewDecorated) override;

		virtual void SetFullscreen_Impl(bool bNewFullscreen) override;

	private:
		GLFWwindow* Window{};
	};
}
#endif