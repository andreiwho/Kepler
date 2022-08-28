#include "SoundMA.h"
#include "AudioEngineMA.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogSoundMA);

	TSoundMA::TSoundMA(const TString& InPath, const ESoundCreateFlags CreateFlags)
		:	TSound(InPath, CreateFlags)
	{
		// We need to avoid the thread lock if the sound is streamed
		ESoundCreateFlags ActualFlags = CreateFlags;
		ma_uint32 LoadFlags = 0;
		
		TAudioEngineMA* Engine = (TAudioEngineMA*)TAudioEngine::Get();
		u32 MaxBufferCount = Engine->MaxOverlappingClips;

		if (ActualFlags & ESoundCreateFlags::Streamed)
		{
			ActualFlags.Mask &= ~ESoundCreateFlags::Async;
			LoadFlags |= MA_SOUND_FLAG_STREAM;

			MaxBufferCount = 1;
		}

		// If we are still async, create the fence
		if (ActualFlags & ESoundCreateFlags::Async)
		{
			DoneFence = (ma_fence*)TMalloc::Get()->Allocate(sizeof(ma_fence));
			MACHECK(ma_fence_init(DoneFence));
			LoadFlags |= MA_SOUND_FLAG_ASYNC;
		}

		if (ActualFlags & ESoundCreateFlags::Decode)
		{
			LoadFlags |= MA_SOUND_FLAG_DECODE;
		}
		CHECK(MaxBufferCount > 0);
		SoundBuffer.Resize(MaxBufferCount);
		for (u32 Index = 0; Index < MaxBufferCount; ++Index)
		{
			MACHECK(ma_sound_init_from_file(Engine->GetEngineHandle(), InPath.c_str(), LoadFlags, nullptr, DoneFence, &SoundBuffer[Index]));
		}

		if (CreateFlags & ESoundCreateFlags::Looping)
		{
			SetLooping(true);
		}

		// TODO: Deal with looping
		KEPLER_TRACE(LogSoundMA, "Audio track {} loaded", InPath);
		CurrentSound = MaxBufferCount - 1;
	}

	TSoundMA::~TSoundMA()
	{
		KEPLER_TRACE(LogSoundMA, "Unloading audio track {}", Path);

		TAudioEngineMA* Engine = (TAudioEngineMA*)TAudioEngine::Get();
		for (auto& Sound : SoundBuffer)
		{
			ma_sound_uninit(&Sound);
		}
	}

	bool TSoundMA::IsPlaying() const
	{
		return ma_sound_is_playing(&SoundBuffer[CurrentSound]);
	}

	bool TSoundMA::IsFinished() const
	{
		return ma_sound_at_end(&SoundBuffer[CurrentSound]);
	}

	void TSoundMA::Rewind()
	{
		ma_sound_seek_to_pcm_frame(&SoundBuffer[CurrentSound], 0);
	}

	void TSoundMA::Play()
	{
		CurrentSound = (CurrentSound + 1) % (u32)SoundBuffer.GetLength();
		WaitForLoad();

		if (IsPlaying())
		{
			Stop();
			Rewind();
		}

		if (bIsLooping)
		{
			ma_sound_set_looping(&SoundBuffer[CurrentSound], true);
		}
		ma_sound_start(&SoundBuffer[CurrentSound]);
	}

	void TSoundMA::WaitForLoad()
	{
		if (DoneFence)
		{
			ma_fence_wait(DoneFence);
			ma_fence_uninit(DoneFence);
			TMalloc::Get()->Free(DoneFence);
			DoneFence = nullptr;
		}
	}

	void TSoundMA::Stop()
	{
		ma_sound_stop(&SoundBuffer[CurrentSound]);
	}
}