#pragma once
#include "Core/Core.h"
#include "AudioCommon.h"

namespace Kepler
{
	class TAudioEngine
	{
		static TAudioEngine* Instance;
	
	protected:
		TAudioEngine();

	public:
		virtual ~TAudioEngine() = default;

		static TAudioEngine* Get() { return Instance; }

		static TSharedPtr<TAudioEngine> CreateAudioEngine(EAudioEngineAPI Api = EAudioEngineAPI::Default);
	};
}