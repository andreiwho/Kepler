#pragma once
#include <chrono>
namespace Kepler
{
	class TTimer
	{
    public:
		TTimer();
		void Begin();
		void End();

		float Delta() const;
		
	private:
		double StartTime{};
		float DeltaTime{};
	};

	extern TTimer* GGlobalTimer;
}