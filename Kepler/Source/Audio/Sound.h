#pragma once
#include "Core/Core.h"
#include "AudioCommon.h"

namespace ke
{
	class TSound : public IntrusiveRefCounted
	{
	protected:
		TSound(const TString& path, ESoundCreateFlags flags = ESoundCreateFlags::None);

	public:
		virtual bool IsPlaying() const = 0;
		virtual bool IsFinished() const = 0;
		virtual void Rewind() = 0;
		virtual void WaitForLoad() = 0;
		virtual void Play(float3 position = float3()) = 0;
		virtual void Stop() = 0;
		virtual void SetLooping(bool bLooping)
		{
			m_bIsLooping = bLooping;
		}

		static TRef<TSound> New(const TString& path, ESoundCreateFlags flags = ESoundCreateFlags::None);
	
	protected:
		TString m_Path{};
		bool m_bIsLooping = false;
	};
}