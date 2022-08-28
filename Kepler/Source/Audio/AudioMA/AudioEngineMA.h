#pragma once
#include "Audio/AudioEngine.h"
#include "Core/Containers/ChaosMap.h"
#include "MACommon.h"
#include "SoundMA.h"

namespace Kepler
{
	class TAudioEngineMA : public TAudioEngine
	{
	public:
		TAudioEngineMA();
		~TAudioEngineMA();

		inline ma_engine* GetEngineHandle() { return &Engine; }

		virtual void PlayInline(const TString& Path) override;

		virtual void Play(const TString& Path, ESoundCreateFlags LoadFlags) override;

		virtual void PlayAt(const TString& Path, float3 Position, ESoundCreateFlags LoadFlags = 0) override;

		virtual void UnloadPlaybackCache(bool bAlsoForPlaying) override;
	
		virtual TRef<TSound> GetOrLoadSound(const TString& InPath, ESoundCreateFlags LoadFlags = 0) override;

	private:
		ma_engine Engine;

		TChaoticMap<TString, TRef<TSoundMA>> Sounds;
	};
}