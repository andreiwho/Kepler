#include "Sound.h"
#include "Audio/AudioMA/SoundMA.h"

namespace ke
{

	TSound::TSound(const TString& InPath, ESoundCreateFlags InCreateFlags)
		:	Path(InPath)
	{
	}

	TRef<TSound> TSound::New(const TString& InPath, ESoundCreateFlags CreateFlags)
	{
		switch (GAudioEngineAPI)
		{
		case ke::EAudioEngineAPI::MiniAudio:
			return MakeRef(ke::New<TSoundMA>(InPath, CreateFlags));
		}
		return nullptr;
	}

}