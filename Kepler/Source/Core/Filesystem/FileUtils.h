#pragma once
#include "Core/Types.h"
#include "Async/Async.h"
#include <string>
#include "Reflection/Class.h"

namespace ke
{
	struct TFileUtils
	{
		static std::future<String> ReadTextFileAsync(const String& path, bool bResolved = false);
		static TFuture<void> WriteTextFileAsync(const String& path, const String& text);
		static std::future<Array<u8>> ReadBinaryFileAsync(const String& path);
		static bool PathExists(const String& path);
	};

	struct FilePickers
	{
		static bool OpenAssetPicker(String& outResult, EFieldAssetType assetType);
		static bool SaveAssetPicker(String& outResult, EFieldAssetType assetType);
	};
}