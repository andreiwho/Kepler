#include "WindowGLFW.h"

#ifdef PLATFORM_DESKTOP
#include "Platform/Platform.h"

#ifdef WIN32
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GET_NATIVE_WINDOW(window) glfwGetWin32Window(window)
#endif
#include <GLFW/glfw3native.h>
#include <cassert>

DEFINE_UNIQUE_LOG_CHANNEL(LogGLFWWindow, All);

namespace ke
{
	TWindowGLFW::TWindowGLFW(i32 width, i32 height, const TString& title, const TWindowParams& params)
		: TWindow(width, height, title, params)
	{
		glfwWindowHint(GLFW_DECORATED, params.bDecorated);
		glfwWindowHint(GLFW_MAXIMIZED, params.bMaximized);

		m_Window = glfwCreateWindow(m_Width, m_Height, title.c_str(), params.bFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		assert(m_Window && "Failed to create GLFW window");

		glfwGetFramebufferSize(m_Window, &m_Width, &m_Height);

		SetupCallbacks();
	}

	TWindowGLFW::~TWindowGLFW()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
		}
	}

	void* TWindowGLFW::GetNativeHandle() const
	{
		return GET_NATIVE_WINDOW(m_Window);
	}

	void TWindowGLFW::RequestClose()
	{
		m_bCloseRequested = true;
	}

	void TWindowGLFW::Internal_UpdateSize(i32 width, i32 heigth)
	{
		m_Width = width;
		m_Height = heigth;
	}

	void TWindowGLFW::SetCursorPosition(float2 newPos)
	{
		glfwSetCursorPos(m_Window, (float)newPos.x, (float)newPos.y);
	}

	void TWindowGLFW::SetupCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				win->RequestClose();
				TPlatform::Get()->OnPlatformEvent(TWindowClosedEvent(win));
			});

		/************************************************************************/
		/* MOUSE EVENTS                                                         */
		/************************************************************************/
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TMouseMoveEvent(win, (float)x, (float)y));
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
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

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double, double vertical)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TMouseScrollWheelEvent(win, (float)vertical));
			});

		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
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
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int, int action, int)
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

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TCharEvent(win, static_cast<char>(character)));
			});

		/************************************************************************/
		/* WINDOW EVENTS                                                        */
		/************************************************************************/
		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, i32 x, i32 y)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				TPlatform::Get()->OnPlatformEvent(TWindowMoveEvent(win, x, y));
			});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, i32 width, i32 height)
			{
				TWindowGLFW* win = (TWindowGLFW*)glfwGetWindowUserPointer(window);
				win->Internal_UpdateSize(width, height);
				TPlatform::Get()->OnPlatformEvent(TWindowSizeEvent(win, width, height));
			});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
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

		glfwSetWindowFocusCallback(m_Window, 
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

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
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

	void TWindowGLFW::SetTitle_Impl(const TString& newTitle)
	{
		glfwSetWindowTitle(m_Window, newTitle.c_str());
	}

	void TWindowGLFW::SetMaximized_Impl(bool bNewMaximized)
	{
		if (bNewMaximized)
		{
			glfwMaximizeWindow(m_Window);
		}
		glfwRestoreWindow(m_Window);
	}

	void TWindowGLFW::SetDecorated_Impl(bool bNewDecorated)
	{
		glfwSetWindowAttrib(m_Window, GLFW_DECORATED, bNewDecorated ? GLFW_TRUE : GLFW_FALSE);
	}

	void TWindowGLFW::SetFullscreen_Impl(bool bNewFullscreen)
	{
		// TODO: This may work bad. Consider setting size, refresh rate and position
		glfwSetWindowMonitor(m_Window, bNewFullscreen ? glfwGetPrimaryMonitor() : nullptr, 32, 32, m_Width, m_Height, GLFW_DONT_CARE);
	}
}
#endif