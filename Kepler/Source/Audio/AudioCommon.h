#pragma once
#include "Core/Core.h"
#include "Core/Macros.h"

namespace Kepler
{
	enum class EAudioEngineAPI
	{
		MiniAudio,
		Default = MiniAudio
	};

	DEFINE_UNIQUE_LOG_CHANNEL(LogAudio);
}