#pragma once
#include "Audio/Sound.h"
#include "MACommon.h"

namespace ke
{
	class TSoundMA : public TSound
	{
	public:
		TSoundMA(const TString& path, ESoundCreateFlags flags = ESoundCreateFlags::None);
		~TSoundMA();

		virtual bool IsPlaying() const override;
		virtual bool IsFinished() const override;
		virtual void Rewind() override;
		virtual void Play(float3 position = float3()) override;
		virtual void WaitForLoad() override;
		virtual void Stop() override;

	private:
		TDynArray<ma_sound> m_SoundBuffer;
		ma_fence* m_DoneFence{nullptr};
		u32 m_CurrentSound = 0;
	};
}