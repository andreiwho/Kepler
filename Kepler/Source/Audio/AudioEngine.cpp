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
	ke::TAudioEngine* TAudioEngine::Instance;

	TAudioEngine::TAudioEngine()
	{
		Instance = this;
	}

	TSharedPtr<TAudioEngine> TAudioEngine::CreateAudioEngine(EAudioEngineAPI api)
	{
		switch (api)
		{
		case ke::EAudioEngineAPI::MiniAudio:
			GAudioEngineAPI = EAudioEngineAPI::MiniAudio;
			return MakeShared<TAudioEngineMA>();
			break;
		default:
			break;
		}
		return nullptr;
	}
}