#include "ImageLoader.h"

#include <stb/stb_image.h>
#include "Core/Macros.h"
#include "Renderer/RenderGlobals.h"
#include "Renderer/Elements/Image.h"
#include "Renderer/LowLevelRenderer.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogImageLoader, All);

	TImageLoader* TImageLoader::Instance = nullptr;

	TImageLoader::TImageLoader()
	{
		Instance = this;
	}

	TImageLoader::~TImageLoader()
	{
		ClearCache();
	}

	void TImageLoader::ClearCache()
	{
		LoadedSamplers.Clear();
	}

	std::future<TImageData> TImageLoader::Load(const TString& Path)
	{
		return Async(
			[CopiedPath = VFSResolvePath(Path)]
			{
				static constexpr u32 COMPONENT_COUNT = 4;
				KEPLER_TRACE(LogImageLoader, "Loading image {}", CopiedPath);
				i32 Width, Height, Comp;
				// Todo: move to using TMallocator for this
				u8* Data = stbi_load(CopiedPath.c_str(), &Width, &Height, &Comp, (int)COMPONENT_COUNT);
				CHECK(Data);
				Array<u8> DataArray{ Data, Data + Width * Height * 4 };
				stbi_image_free(Data);

				return TImageData
				{
					IAsyncDataBlob::New(DataArray),
					(u32)Width,
					(u32)Height,
					COMPONENT_COUNT
				};
			});
	}

	RefPtr<ITextureSampler2D> TImageLoader::LoadSamplerCached(const TString& Path)
	{
		CHECK(!IsRenderThread());

		if (LoadedSamplers.Contains(Path))
		{
			return LoadedSamplers[Path];
		}

		auto ImageData = Await(Load(Path));
		auto Task = TRenderThread::Submit([&] 
			{
				auto SampledImage = IImage2D::New(ImageData.Width, ImageData.Height, EFormat::R8G8B8A8_UNORM, EImageUsage::ShaderResource);
				SampledImage->Write(LowLevelRenderer::Get()->GetRenderDevice()->GetImmediateCommandList(), 0, 0, ImageData.Width, ImageData.Height, ImageData.Data);
				return ITextureSampler2D::New(SampledImage);
			});
		LoadedSamplers[Path] = Await(Task);
		return LoadedSamplers[Path];
	}

}