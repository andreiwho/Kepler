#pragma once
#include "Audio/AudioEngine.h"
#include "Core/Containers/ChaosMap.h"
#include "MACommon.h"
#include "SoundMA.h"

namespace ke
{
	class AudioEngineMA : public AudioEngine
	{
	public:
		AudioEngineMA();
		~AudioEngineMA();

		inline ma_engine* GetEngineHandle() { return &m_Engine; }
		virtual void PlayInline(const TString& path) override;
		virtual void Play(const TString& path, ESoundCreateFlags flags) override;
		virtual void PlayAt(const TString& path, float3 position, ESoundCreateFlags flags = 0) override;
		virtual void UnloadPlaybackCache(bool bAlsoForPlaying) override;
		virtual RefPtr<TSound> GetOrLoadSound(const TString& path, ESoundCreateFlags flags = 0) override;

	private:
		ma_engine m_Engine;
		Map<TString, RefPtr<TSoundMA>> m_Sounds;
	};
}