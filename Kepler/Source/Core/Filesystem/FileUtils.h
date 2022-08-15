#pragma once
#include "Core/Types.h"
#include "Async/Async.h"
#include <string>

namespace Kepler
{
	struct TFileUtils
	{
		static std::future<std::string> ReadTextFileAsync(const std::string& Path);
	};
}