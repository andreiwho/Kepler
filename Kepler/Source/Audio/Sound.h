#pragma once
#include "Core/Core.h"
#include "AudioCommon.h"

namespace Kepler
{
	class TSound : public TRefCounted
	{
	protected:
		TSound(const TString& InPath, ESoundCreateFlags CreateFlags = ESoundCreateFlags::None);

	public:
		virtual bool IsPlaying() const = 0;
		virtual bool IsFinished() const = 0;
		virtual void Rewind() = 0;
		virtual void WaitForLoad() = 0;
		virtual void Play() = 0;
		virtual void Stop() = 0;

		static TRef<TSound> New(const TString& InPath, ESoundCreateFlags CreateFlags = ESoundCreateFlags::None);
	
	protected:
		ESoundCreateFlags CreateFlags;
	};
}