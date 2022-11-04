#include "Sound.h"
#include "Audio/AudioMA/SoundMA.h"

namespace ke
{

	TSound::TSound(const String& path, ESoundCreateFlags flags)
		:	m_Path(path)
	{
	}

	RefPtr<TSound> TSound::New(const String& path, ESoundCreateFlags flags)
	{
		switch (GAudioEngineAPI)
		{
		case ke::EAudioEngineAPI::MiniAudio:
			return MakeRef(ke::New<TSoundMA>(path, flags));
		}
		return nullptr;
	}

}