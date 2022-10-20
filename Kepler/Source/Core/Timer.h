#pragma once
#include <chrono>
namespace ke
{
	class TTimer
	{
    public:
		TTimer();
		void Begin();
		void End();

		float Delta() const;
		static float GetTimeSeconds();

	private:
		double m_StartTime{};
		float m_DeltaTime{};
	};

	extern TTimer* GGlobalTimer;
}