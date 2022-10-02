#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioEngineMA.h"

namespace ke
{

	TAudioEngineMA::TAudioEngineMA()
	{
		MACHECK(ma_engine_init(nullptr, &Engine));
		KEPLER_INFO(LogAudio, "MiniAudio Engine initialized");
	}

	TAudioEngineMA::~TAudioEngineMA()
	{
		Sounds.Clear();
		ma_engine_uninit(&Engine);
		KEPLER_INFO(LogAudio, "MiniAudio Engine terminated");
	}

	void TAudioEngineMA::PlayInline(const TString& Path)
	{
		auto ResolvedPath = VFSResolvePath(Path);
		ma_engine_play_sound(&Engine, ResolvedPath.c_str(), nullptr);
	}

	void TAudioEngineMA::Play(const TString& Path, ESoundCreateFlags LoadFlags)
	{
		GetOrLoadSound(Path, LoadFlags)->Play();
	}

	void TAudioEngineMA::PlayAt(const TString& Path, float3 Position, ESoundCreateFlags LoadFlags)
	{
		GetOrLoadSound(Path, LoadFlags)->Play(Position);
	}

	void TAudioEngineMA::UnloadPlaybackCache(bool bAlsoForPlaying)
	{
		if (bAlsoForPlaying)
		{
			Sounds.Clear();
		}
		else
		{
			std::erase_if(Sounds.GetUnderlyingContainer(),
				[](const auto& Sound) 
				{
					return !Sound.second->IsPlaying();
				});
		}
	}

	TRef<TSound> TAudioEngineMA::GetOrLoadSound(const TString& InPath, ESoundCreateFlags LoadFlags)
	{
		if (!Sounds.Contains(InPath))
		{
			TString ResolvedPath = VFSResolvePath(InPath);
			Sounds[InPath] = RefCast<TSoundMA>(TSound::New(ResolvedPath, ESoundCreateFlags::Decode | ESoundCreateFlags::Async | LoadFlags.Mask));
		}
		return Sounds[InPath];
	}

}