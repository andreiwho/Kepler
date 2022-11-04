#include "AudioEngine.h"
#include "AudioCommon.h"
#include "miniaudio.h"

#include "Audio/AudioMA/AudioEngineMA.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	ke::AudioEngine* AudioEngine::Instance;

	AudioEngine::AudioEngine()
	{
		Instance = this;
	}

	SharedPtr<AudioEngine> AudioEngine::CreateAudioEngine(EAudioEngineAPI api)
	{
		switch (api)
		{
		case ke::EAudioEngineAPI::MiniAudio:
			GAudioEngineAPI = EAudioEngineAPI::MiniAudio;
			return MakeShared<AudioEngineMA>();
			break;
		default:
			break;
		}
		return nullptr;
	}
}