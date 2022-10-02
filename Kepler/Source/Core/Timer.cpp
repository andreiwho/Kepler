#include "Timer.h"
#ifdef PLATFORM_DESKTOP
# include <GLFW/glfw3.h>
#endif
namespace ke
{
	TTimer* GGlobalTimer = nullptr;
	
	TTimer::TTimer()
	{
		Begin();
	}

	void TTimer::Begin()
	{
#ifdef PLATFORM_DESKTOP
		StartTime = glfwGetTime();
#else
#endif
	}

	void TTimer::End()
	{
		double EndTime = 0.0;
#ifdef PLATFORM_DESKTOP
		EndTime = glfwGetTime();
#else
#endif
		DeltaTime = (float)EndTime - (float)StartTime;
	}

	float TTimer::Delta() const
	{
		return DeltaTime;
	}

}