#pragma once
#include "Core/Core.h"
#include "Core/Macros.h"

namespace ke
{
	enum class EAudioEngineAPI
	{
		MiniAudio,
		Default = MiniAudio
	};

	extern EAudioEngineAPI GAudioEngineAPI;

	struct ESoundCreateFlags
	{
		enum Type
		{
			None = 0,
			Async = BIT(0),
			Decode = BIT(1),
			Streamed = BIT(2),
			Looping = BIT(3),
		};

		u32 Mask = 0;
		constexpr ESoundCreateFlags(u32 mask) : Mask(mask) {}
		inline constexpr operator u32() const { return Mask; }
	};

	DEFINE_UNIQUE_LOG_CHANNEL(LogAudio);
}