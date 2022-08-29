#include "ImageLoader.h"

#include <stb/stb_image.h>

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogImageLoader);

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
				TDynArray<u8> DataArray{ Data, Data + Width * Height * 4 };
				stbi_image_free(Data);

				return TImageData
				{
					TDataBlob::New(DataArray),
					(u32)Width,
					(u32)Height,
					COMPONENT_COUNT
				};
			});
	}
}