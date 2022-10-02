#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioEngineMA.h"

namespace ke
{

	TAudioEngineMA::TAudioEngineMA()
	{
		MACHECK(ma_engine_init(nullptr, &m_Engine));
		KEPLER_INFO(LogAudio, "MiniAudio Engine initialized");
	}

	TAudioEngineMA::~TAudioEngineMA()
	{
		m_Sounds.Clear();
		ma_engine_uninit(&m_Engine);
		KEPLER_INFO(LogAudio, "MiniAudio Engine terminated");
	}

	void TAudioEngineMA::PlayInline(const TString& path)
	{
		ma_engine_play_sound(&m_Engine, VFSResolvePath(path).c_str(), nullptr);
	}

	void TAudioEngineMA::Play(const TString& path, ESoundCreateFlags flags)
	{
		GetOrLoadSound(path, flags)->Play();
	}

	void TAudioEngineMA::PlayAt(const TString& path, float3 position, ESoundCreateFlags flags)
	{
		GetOrLoadSound(path, flags)->Play(position);
	}

	void TAudioEngineMA::UnloadPlaybackCache(bool bAlsoForPlaying)
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

	TRef<TSound> TAudioEngineMA::GetOrLoadSound(const TString& path, ESoundCreateFlags flags)
	{
		if (!m_Sounds.Contains(path))
		{
			m_Sounds[path] = RefCast<TSoundMA>(TSound::New(VFSResolvePath(path), ESoundCreateFlags::Decode | ESoundCreateFlags::Async | flags.Mask));
		}
		return m_Sounds[path];
	}

}