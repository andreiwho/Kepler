#include "WindowGLFW.h"

#ifdef PLATFORM_DESKTOP
#include "Platform/Platform.h"

#ifdef WIN32
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GET_NATIVE_WINDOW(window) glfwGetWin32Window(window)
#endif
#include <GLFW/glfw3native.h>
#include <cassert>

DEFINE_UNIQUE_LOG_CHANNEL(LogGLFWWindow);

namespace Kepler
{
	TWindowGLFW::TWindowGLFW(i32 InWidth, i32 InHeight, const TString& InTitle, const TWindowParams& Params)
		: TWindow(InWidth, InHeight, InTitle, Params)
	{
		glfwWindowHint(GLFW_DECORATED, Params.bDecorated);
		glfwWindowHint(GLFW_MAXIMIZED, Params.bMaximized);

		Window = glfwCreateWindow(Width, Height, InTitle.c_str(), Params.bFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		assert(Window && "Failed to create GLFW window");

		glfwGetFramebufferSize(Window, &Width, &Height);

		SetupCallbacks();
	}

	TWindowGLFW::~TWindowGLFW()
	{
		if (Window)
		{
			glfwDestroyWindow(Window);
		}
	}

	void* TWindowGLFW::GetNativeHandle() const
	{
		return GET_NATIVE_WINDOW(Window);
	}

	void TWindowGLFW::RequestClose()
	{
		bCloseRequested = true;
	}

	void TWindowGLFW::Internal_UpdateSize(i32 InWidth, i32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
	}

	void TWindowGLFW::SetCursorPosition(float2 NewPosition)
	{
		glfwSetCursorPos(Window, (float)NewPosition.x, (float)NewPosition.y);
	}

	void TWindowGLFW::SetupCallbacks()
	{
		glfwSetWindowUserPointer(Window, this);

		glfwSetWindowCloseCallback(Window, [](GLFWwindow* window)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				win->RequestClose();
				TPlatform::Get()->OnPlatformEvent(TWindowClosedEvent(win));
			});

		/************************************************************************/
		/* MOUSE EVENTS                                                         */
		/************************************************************************/
		glfwSetCursorPosCallback(Window, [](GLFWwindow* window, double x, double y)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TMouseMoveEvent(win, (float)x, (float)y));
			});

		glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);

				const EMouseButton::EValue ActualButton = std::invoke([button]
					{
						switch (button)
						{
						case GLFW_MOUSE_BUTTON_LEFT:
							return EMouseButton::Left;
						case GLFW_MOUSE_BUTTON_RIGHT:
							return EMouseButton::Right;
						case GLFW_MOUSE_BUTTON_MIDDLE:
							return EMouseButton::Middle;
						}
						return EMouseButton::Unknown;
					});

				if (action == GLFW_PRESS)
				{
					TPlatform::Get()->OnPlatformEvent(TMouseButtonDownEvent(win, static_cast<EMouseButton::EValue>(ActualButton)));
				}
				else
				{
					TPlatform::Get()->OnPlatformEvent(TMouseButtonUpEvent(win, static_cast<EMouseButton::EValue>(ActualButton)));
				}
			});

		glfwSetScrollCallback(Window, [](GLFWwindow* window, double, double vertical)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TMouseScrollWheelEvent(win, (float)vertical));
			});

		glfwSetCursorEnterCallback(Window, [](GLFWwindow* window, int entered)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				if (entered)
				{
					TPlatform::Get()->OnPlatformEvent(TMouseEnterEvent(win));
				}
				else
				{
					TPlatform::Get()->OnPlatformEvent(TMouseLeaveEvent(win));
				}
			});

		/************************************************************************/
		/* KEYBOARD EVENTS                                                      */
		/************************************************************************/
		glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int, int action, int)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
					TPlatform::Get()->OnPlatformEvent(TKeyDownEvent(win, (EKeyCode::EValue)key));
					break;
				case GLFW_RELEASE:
					TPlatform::Get()->OnPlatformEvent(TKeyUpEvent(win, (EKeyCode::EValue)key));
					break;
				case GLFW_REPEAT:
					TPlatform::Get()->OnPlatformEvent(TKeyHoldEvent(win, (EKeyCode::EValue)key));
					break;
				}
			});

		glfwSetCharCallback(Window, [](GLFWwindow* window, unsigned int character)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TCharEvent(win, static_cast<char>(character)));
			});

		/************************************************************************/
		/* WINDOW EVENTS                                                        */
		/************************************************************************/
		glfwSetWindowPosCallback(Window, [](GLFWwindow* window, i32 x, i32 y)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TWindowMoveEvent(win, x, y));
			});

		glfwSetFramebufferSizeCallback(Window, [](GLFWwindow* window, i32 width, i32 height)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				win->Internal_UpdateSize(width, height);
				TPlatform::Get()->OnPlatformEvent(TWindowSizeEvent(win, width, height));
			});

		glfwSetWindowIconifyCallback(Window, [](GLFWwindow* window, int iconified)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				if (iconified)
				{
					TPlatform::Get()->OnPlatformEvent(TWindowMinimizeEvent(win));
				}
				else
				{
					TPlatform::Get()->OnPlatformEvent(TWindowRestoreEvent(win));
				}
			});

		glfwSetWindowFocusCallback(Window, 
			[](GLFWwindow* window, int focused)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				if (focused)
				{
					TPlatform::Get()->OnPlatformEvent(TWindowFocusedEvent(win));
				}
				else
				{
					TPlatform::Get()->OnPlatformEvent(TWindowUnfocusedEvent(win));
				}
			});

		glfwSetWindowMaximizeCallback(Window, [](GLFWwindow* window, int maximized)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				if (maximized)
				{
					TPlatform::Get()->OnPlatformEvent(TWindowMaximizeEvent(win));
				}
				else
				{
					TPlatform::Get()->OnPlatformEvent(TWindowRestoreEvent(win));
				}
			});
	}

	void TWindowGLFW::SetTitle_Impl(const TString& NewTitle)
	{
		glfwSetWindowTitle(Window, NewTitle.c_str());
	}

	void TWindowGLFW::SetMaximized_Impl(bool bNewMaximized)
	{
		if (bNewMaximized)
		{
			glfwMaximizeWindow(Window);
		}
		glfwRestoreWindow(Window);
	}

	void TWindowGLFW::SetDecorated_Impl(bool bNewDecorated)
	{
		glfwSetWindowAttrib(Window, GLFW_DECORATED, bNewDecorated ? GLFW_TRUE : GLFW_FALSE);
	}

	void TWindowGLFW::SetFullscreen_Impl(bool bNewFullscreen)
	{
		// TODO: This may work bad. Consider setting size, refresh rate and position
		glfwSetWindowMonitor(Window, bNewFullscreen ? glfwGetPrimaryMonitor() : nullptr, 32, 32, Width, Height, GLFW_DONT_CARE);
	}

}
#endif