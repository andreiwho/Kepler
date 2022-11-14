#pragma once
#include "Build.h"
#include <chrono>

namespace ke
{
	class CORE_API TTimer
	{
		using ClockType = std::chrono::high_resolution_clock;
    public:
		TTimer();
		void Begin();
		void End();

		float Delta() const;
		float GetTimeSeconds() const;

	private:
		ClockType m_Clock;
		ClockType::time_point m_StartTime{};
		ClockType::time_point m_EndTime{};

		float m_DeltaTime{};
	};

	CORE_API extern TTimer* GGlobalTimer;
}