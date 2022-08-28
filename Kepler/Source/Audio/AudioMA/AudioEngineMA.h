#pragma once
#include "Audio/AudioEngine.h"
#include "MACommon.h"

namespace Kepler
{
	class TAudioEngineMA : public TAudioEngine
	{
	public:
		TAudioEngineMA();
		~TAudioEngineMA();

		inline ma_engine* GetEngineHandle() { return &Engine; }

	private:
		ma_engine Engine;
	};
}