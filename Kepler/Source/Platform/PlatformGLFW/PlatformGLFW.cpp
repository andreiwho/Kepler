#include "PlatformGLFW.h"
#ifdef PLATFORM_DESKTOP
#include "Core/Log.h"
#include "WindowGLFW.h"

#include <cassert>

#include <GLFW/glfw3.h>

#ifdef CreateWindow
#  undef CreateWindow
#endif

namespace Kepler
{
	TPlatformGLFW::TPlatformGLFW()
	{
		bInitialized = !!glfwInit();
		KEPLER_INFO("LogPlatform", "GLFW platform initialized");
		if (!bInitialized)
		{
			// Throw some kind of an error
		}

		// Specify that we will not be using OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	TPlatformGLFW::~TPlatformGLFW()
	{
		if (bInitialized)
		{
			glfwTerminate();
			KEPLER_INFO("LogPlatform", "GLFW platform terminated");
		}
	}

	TWindow* TPlatformGLFW::CreatePlatformWindow(i32 Width, i32 Height, const std::string& Title, const TWindowParams& Params)
	{
		KEPLER_INFO("LogPlatform", "Creating GLFW platform window '{}'", Title);
		return Windows.emplace_back(std::make_unique<TWindowGLFW>(Width, Height, Title, Params)).get();
	}

	void TPlatformGLFW::Update()
	{
		glfwPollEvents();
		DestroyClosedWindows();

		if (!HasActiveMainWindow())
		{
			Terminate();
			return;
		}
	}

	bool TPlatformGLFW::HasActiveMainWindow() const
	{
		return !Windows.empty() && !!Windows[0];
	}

	void TPlatformGLFW::OnPlatformEvent(const TPlatformEventBase& event)
	{
		// Handle the event if needed
		// ...
		// After handling pass this one to the GenericPlatformInterface
		TPlatform::OnPlatformEvent(event);
	}

	void TPlatformGLFW::DestroyClosedWindows()
	{
		for (auto& window : Windows)
		{
			if (window && window->IsCloseRequested())
			{
				window.reset();
			}
		}
	}

	void TPlatformGLFW::Terminate()
	{
		for (auto& window : Windows)
		{
			window.reset();
		}
	}

	std::shared_ptr<TPlatform> TPlatform::CreatePlatformInterface()
	{
		return std::make_shared<TPlatformGLFW>();
	}
}

#endif