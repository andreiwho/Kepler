#pragma once
#include <chrono>

namespace ke
{
	class TTimer
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

	extern TTimer* GGlobalTimer;
}