#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioEngineMA.h"

namespace Kepler
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
		auto ResolvedPath = VFSResolvePath(Path);
		if (!Sounds.Contains(Path))
		{
			Sounds[Path] = RefCast<TSoundMA>(TSound::New(ResolvedPath, ESoundCreateFlags::Decode | ESoundCreateFlags::Async | LoadFlags.Mask));
		}

		TRef<TSound> Sound = Sounds[Path];
		Sound->Play();
	}

}