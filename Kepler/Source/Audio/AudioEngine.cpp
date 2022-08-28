#include "AudioEngine.h"
#include "AudioCommon.h"
#include "miniaudio.h"

#include "Audio/AudioMA/AudioEngineMA.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	Kepler::TAudioEngine* TAudioEngine::Instance;

	TAudioEngine::TAudioEngine()
	{
		Instance = this;
	}

	TSharedPtr<TAudioEngine> TAudioEngine::CreateAudioEngine(EAudioEngineAPI Api)
	{
		switch (Api)
		{
		case Kepler::EAudioEngineAPI::MiniAudio:
			return MakeShared<TAudioEngineMA>();
			break;
		default:
			break;
		}
		return nullptr;
	}
}