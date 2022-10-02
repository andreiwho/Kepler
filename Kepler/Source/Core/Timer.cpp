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
		m_StartTime = glfwGetTime();
#else
#endif
	}

	void TTimer::End()
	{
		double endTime = 0.0;
#ifdef PLATFORM_DESKTOP
		endTime = glfwGetTime();
#else
#endif
		m_DeltaTime = (float)endTime - (float)m_StartTime;
	}

	float TTimer::Delta() const
	{
		return m_DeltaTime;
	}

}