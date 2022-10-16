#pragma once
#include "Core/Core.h"
#include "Core/Types.h"
#include "Async/Async.h"
#include "Renderer/RenderTypes.h"

namespace ke
{
	struct TImageData
	{
		RefPtr<AsyncDataBlob> Data;
		u32 Width, Height, Components;
	};

	class TTextureSampler2D;
	class TImageLoader
	{
		static TImageLoader* Instance;
	public:
		TImageLoader();
		~TImageLoader();

		void ClearCache();

		static TImageLoader* Get() { return Instance; }

		// This is the old version, which should not be used if the loaded sampler is your goal.
		static std::future<TImageData> Load(const TString& Path);

		RefPtr<TTextureSampler2D> LoadSamplerCached(const TString& Path);

	private:
		Map<TString, RefPtr<TTextureSampler2D>> LoadedSamplers;
	};
}