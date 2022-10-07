#pragma once
#include "Core/Types.h"
#include "Async/Async.h"
#include <string>

namespace ke
{
	struct TFileUtils
	{
		static std::future<TString> ReadTextFileAsync(const TString& path);
		static TFuture<void> WriteTextFileAsync(const TString& path, const TString& text);
		static std::future<Array<u8>> ReadBinaryFileAsync(const TString& path);
		static bool PathExists(const TString& path);
	};
}