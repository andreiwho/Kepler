#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioEngineMA.h"

namespace ke
{

	AudioEngineMA::AudioEngineMA()
	{
		MACHECK(ma_engine_init(nullptr, &m_Engine));
		KEPLER_INFO(LogAudio, "MiniAudio Engine initialized");
	}

	AudioEngineMA::~AudioEngineMA()
	{
		m_Sounds.Clear();
		ma_engine_uninit(&m_Engine);
		KEPLER_INFO(LogAudio, "MiniAudio Engine terminated");
	}

	void AudioEngineMA::PlayInline(const TString& path)
	{
		ma_engine_play_sound(&m_Engine, VFSResolvePath(path).c_str(), nullptr);
	}

	void AudioEngineMA::Play(const TString& path, ESoundCreateFlags flags)
	{
		GetOrLoadSound(path, flags)->Play();
	}

	void AudioEngineMA::PlayAt(const TString& path, float3 position, ESoundCreateFlags flags)
	{
		GetOrLoadSound(path, flags)->Play(position);
	}

	void AudioEngineMA::UnloadPlaybackCache(bool bAlsoForPlaying)
	{
		if (bAlsoForPlaying)
		{
			m_Sounds.Clear();
		}
		else
		{
			std::erase_if(m_Sounds.GetUnderlyingContainer(),
				[](const auto& snd)
				{
					return !snd.second->IsPlaying();
				});
		}
	}

	TRef<TSound> AudioEngineMA::GetOrLoadSound(const TString& path, ESoundCreateFlags flags)
	{
		if (!m_Sounds.Contains(path))
		{
			m_Sounds[path] = RefCast<TSoundMA>(TSound::New(VFSResolvePath(path), ESoundCreateFlags::Decode | ESoundCreateFlags::Async | flags.Mask));
		}
		return m_Sounds[path];
	}

}