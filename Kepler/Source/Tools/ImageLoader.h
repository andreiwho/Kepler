#pragma once
#include "Core/Core.h"
#include "Core/Types.h"
#include "Async/Async.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	struct TImageData
	{
		TRef<TDataBlob> Data;
		u32 Width, Height, Components;
	};

	class TImageLoader
	{
	public:
		static std::future<TImageData> LoadImage(const TString& Path);
	};
}