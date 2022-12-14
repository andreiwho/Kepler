#pragma once
#include "Core/Core.h"
#include "AudioCommon.h"
#include "Sound.h"

namespace ke
{
	class AudioEngine
	{
		static AudioEngine* Instance;

	protected:
		AudioEngine();

	public:
		virtual ~AudioEngine() = default;

		static AudioEngine* Get() { return Instance; }
		static SharedPtr<AudioEngine> CreateAudioEngine(EAudioEngineAPI api = EAudioEngineAPI::Default);
		virtual void PlayInline(const String& path) = 0;
		virtual void Play(const String& path, ESoundCreateFlags flags = 0) = 0;
		virtual void PlayAt(const String& path, float3 position, ESoundCreateFlags flags = 0) = 0;
		virtual RefPtr<TSound> GetOrLoadSound(const String& path, ESoundCreateFlags flags = 0) = 0;
		virtual void UnloadPlaybackCache(bool bAlsoForPlaying) = 0;

	public:
		u32 m_MaxOverlappingClips = 8;
	};
}