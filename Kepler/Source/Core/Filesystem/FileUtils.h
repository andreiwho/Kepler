#pragma once
#include "Core/Types.h"
#include "Async/Async.h"
#include <string>

namespace ke
{
	struct TFileUtils
	{
		static std::future<TString> ReadTextFileAsync(const TString& Path);
		static std::future<TDynArray<u8>> ReadBinaryFileAsync(const TString& Path);
		static bool PathExists(const TString& Path);
	};
}