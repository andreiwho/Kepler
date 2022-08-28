#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioEngineMA.h"

namespace Kepler
{

	TAudioEngineMA::TAudioEngineMA()
	{
		MACHECK(ma_engine_init(nullptr, &Engine));
		KEPLER_INFO(LogAudio, "MiniAudio Engine initialized");
	}

	TAudioEngineMA::~TAudioEngineMA()
	{
		ma_engine_uninit(&Engine);
		KEPLER_INFO(LogAudio, "MiniAudio Engine terminated");
	}

}