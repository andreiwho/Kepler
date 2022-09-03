#pragma once
#include "Audio/Sound.h"
#include "MACommon.h"

namespace Kepler
{
	class TSoundMA : public TSound
	{
	public:
		TSoundMA(const TString& InPath, ESoundCreateFlags CreateFlags = ESoundCreateFlags::None);
		~TSoundMA();

		virtual bool IsPlaying() const override;
		virtual bool IsFinished() const override;
		virtual void Rewind() override;
		virtual void Play(float3 Position = float3()) override;
		virtual void WaitForLoad() override;
		virtual void Stop() override;

	private:
		TDynArray<ma_sound> SoundBuffer;
		ma_fence* DoneFence{nullptr};
		u32 CurrentSound = 0;
	};
}