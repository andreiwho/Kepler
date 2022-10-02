#pragma once
#include "Core/Core.h"
#include "AudioCommon.h"
#include "Sound.h"

namespace ke
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

		virtual void Play(const TString& Path, ESoundCreateFlags LoadFlags = 0) = 0;

		virtual void PlayAt(const TString& Path, float3 Position, ESoundCreateFlags LoadFlags = 0) = 0;

		virtual TRef<TSound> GetOrLoadSound(const TString& InPath, ESoundCreateFlags LoadFlags = 0) = 0;

		virtual void UnloadPlaybackCache(bool bAlsoForPlaying) = 0;

	public:
		u32 MaxOverlappingClips = 8;
	};
}