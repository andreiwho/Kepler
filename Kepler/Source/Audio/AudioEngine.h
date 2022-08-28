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

		virtual void PlayInline(const TString& Path) = 0;

		virtual void Play(const TString& Path, ESoundCreateFlags LoadFlags) = 0;

		virtual void UnloadPlaybackCache(bool bAlsoForPlaying) = 0;

	public:
		u32 MaxOverlappingClips = 8;
	};
}