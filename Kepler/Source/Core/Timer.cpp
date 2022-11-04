#include "Timer.h"
#include <chrono>
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
		m_StartTime = m_Clock.now();
#else
#endif
	}

	void TTimer::End()
	{
		m_EndTime = m_Clock.now();
		auto difference = m_EndTime - m_StartTime;
		auto micros = std::chrono::duration_cast<std::chrono::microseconds>(difference);
		m_DeltaTime = 0.000001f * micros.count();
	}

	float TTimer::Delta() const
	{
		return m_DeltaTime;
	}

	float TTimer::GetTimeSeconds() const
	{
		const auto startTimeMillis = m_StartTime.time_since_epoch();
		return 0.000000001f * startTimeMillis.count();
	}

}