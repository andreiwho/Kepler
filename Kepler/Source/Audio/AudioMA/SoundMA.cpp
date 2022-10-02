#include "SoundMA.h"
#include "AudioEngineMA.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogSoundMA);

	TSoundMA::TSoundMA(const TString& path, const ESoundCreateFlags flags)
		:	TSound(path, flags)
	{
		// We need to avoid the thread lock if the sound is streamed
		ESoundCreateFlags actualFlags = flags;
		ma_uint32 loadFlags = 0;
		
		TAudioEngineMA* pEngine = (TAudioEngineMA*)TAudioEngine::Get();
		u32 numMaxBuffers = pEngine->m_MaxOverlappingClips;

		if (actualFlags & ESoundCreateFlags::Streamed)
		{
			actualFlags.Mask &= ~ESoundCreateFlags::Async;
			loadFlags |= MA_SOUND_FLAG_STREAM;

			numMaxBuffers = 1;
		}

		// If we are still async, create the fence
		if (actualFlags & ESoundCreateFlags::Async)
		{
			m_DoneFence = (ma_fence*)TMalloc::Get()->Allocate(sizeof(ma_fence));
			MACHECK(ma_fence_init(m_DoneFence));
			loadFlags |= MA_SOUND_FLAG_ASYNC;
		}

		if (actualFlags & ESoundCreateFlags::Decode)
		{
			loadFlags |= MA_SOUND_FLAG_DECODE;
		}
		CHECK(numMaxBuffers > 0);
		m_SoundBuffer.Resize(numMaxBuffers);
		for (u32 idx = 0; idx < numMaxBuffers; ++idx)
		{
			MACHECK(ma_sound_init_from_file(pEngine->GetEngineHandle(), path.c_str(), loadFlags, nullptr, m_DoneFence, &m_SoundBuffer[idx]));
		}

		if (flags & ESoundCreateFlags::Looping)
		{
			SetLooping(true);
		}

		// TODO: Deal with looping
		KEPLER_TRACE(LogSoundMA, "Audio track {} loaded", path);
		m_CurrentSound = numMaxBuffers - 1;
	}

	TSoundMA::~TSoundMA()
	{
		KEPLER_TRACE(LogSoundMA, "Unloading audio track {}", m_Path);

		TAudioEngineMA* pEngine = (TAudioEngineMA*)TAudioEngine::Get();
		for (auto& sound : m_SoundBuffer)
		{
			ma_sound_uninit(&sound);
		}
	}

	bool TSoundMA::IsPlaying() const
	{
		return ma_sound_is_playing(&m_SoundBuffer[m_CurrentSound]);
	}

	bool TSoundMA::IsFinished() const
	{
		return ma_sound_at_end(&m_SoundBuffer[m_CurrentSound]);
	}

	void TSoundMA::Rewind()
	{
		ma_sound_seek_to_pcm_frame(&m_SoundBuffer[m_CurrentSound], 0);
	}

	void TSoundMA::Play(float3 position)
	{
		m_CurrentSound = (m_CurrentSound + 1) % (u32)m_SoundBuffer.GetLength();
		WaitForLoad();

		if (IsPlaying())
		{
			Stop();
			Rewind();
		}

		if (m_bIsLooping)
		{
			ma_sound_set_looping(&m_SoundBuffer[m_CurrentSound], true);
		}

		ma_sound_set_position(&m_SoundBuffer[m_CurrentSound], position.x, position.y, position.z);
		ma_sound_start(&m_SoundBuffer[m_CurrentSound]);
	}

	void TSoundMA::WaitForLoad()
	{
		if (m_DoneFence)
		{
			ma_fence_wait(m_DoneFence);
			ma_fence_uninit(m_DoneFence);
			TMalloc::Get()->Free(m_DoneFence);
			m_DoneFence = nullptr;
		}
	}

	void TSoundMA::Stop()
	{
		ma_sound_stop(&m_SoundBuffer[m_CurrentSound]);
	}
}