#include "ImageLoader.h"

#include <stb/stb_image.h>

namespace Kepler
{
	std::future<TImageData> TImageLoader::LoadImage(const TString& Path)
	{
		return Async(
			[CopiedPath = VFSResolvePath(Path)]
			{
				static constexpr u32 COMPONENT_COUNT = 4;

				i32 Width, Height, Comp;
				// Todo: move to using TMallocator for this
				u8* Data = stbi_load(CopiedPath.c_str(), &Width, &Height, &Comp, (int)COMPONENT_COUNT);
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