#pragma once
#include "Core/Core.h"
#include "Core/Types.h"
#include "Async/Async.h"
#include "Renderer/RenderTypes.h"

namespace ke
{
	struct TImageData
	{
		RefPtr<IAsyncDataBlob> Data;
		u32 Width, Height, Components;
	};

	class ITextureSampler2D;
	class TImageLoader
	{
		static TImageLoader* Instance;
	public:
		TImageLoader();
		~TImageLoader();

		void ClearCache();

		static TImageLoader* Get() { return Instance; }

		// This is the old version, which should not be used if the loaded sampler is your goal.
		static std::future<TImageData> Load(const String& Path);

		RefPtr<ITextureSampler2D> LoadSamplerCached(const String& Path);

	private:
		Map<String, RefPtr<ITextureSampler2D>> LoadedSamplers;
	};
}