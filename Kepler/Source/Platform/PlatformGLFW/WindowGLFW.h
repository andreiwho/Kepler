#pragma once
#ifdef PLATFORM_DESKTOP
#include "Platform/Window.h"

#include <GLFW/glfw3.h>

namespace ke
{
	class TWindowGLFW : public TWindow
	{
	public:
		TWindowGLFW(i32 width, i32 height, const TString& title, const TWindowParams& params = {});
		~TWindowGLFW();

		virtual void* GetNativeHandle() const override;
		void RequestClose();
		inline GLFWwindow* GetGLFWWindow() const { return m_Window; }
		void Internal_UpdateSize(i32 width, i32 height);
		virtual void SetCursorPosition(float2 newPosition) override;

	private:
		void SetupCallbacks();

	protected:
		virtual void SetTitle_Impl(const TString& newTitle) override;

		virtual void SetMaximized_Impl(bool bNewMaximized) override;

		virtual void SetDecorated_Impl(bool bNewDecorated) override;

		virtual void SetFullscreen_Impl(bool bNewFullscreen) override;

	private:
		GLFWwindow* m_Window{};
	};
}
#endif