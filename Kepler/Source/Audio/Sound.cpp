#include "Sound.h"
#include "Audio/AudioMA/SoundMA.h"

namespace Kepler
{

	TSound::TSound(const TString& InPath, ESoundCreateFlags InCreateFlags)
		:	CreateFlags(InCreateFlags)
	{
	}

	TRef<TSound> TSound::New(const TString& InPath, ESoundCreateFlags CreateFlags)
	{
		switch (GAudioEngineAPI)
		{
		case Kepler::EAudioEngineAPI::MiniAudio:
			return MakeRef(Kepler::New<TSoundMA>(InPath, CreateFlags));
		}
		return nullptr;
	}

}