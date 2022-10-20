#pragma once
#include "Core/Types.h"
#include "Async/Async.h"
#include <string>

namespace ke
{
	struct TFileUtils
	{
		static std::future<String> ReadTextFileAsync(const String& path);
		static TFuture<void> WriteTextFileAsync(const String& path, const String& text);
		static std::future<Array<u8>> ReadBinaryFileAsync(const String& path);
		static bool PathExists(const String& path);
	};
}