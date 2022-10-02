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
		
	private:
		double StartTime{};
		float DeltaTime{};
	};

	extern TTimer* GGlobalTimer;
}