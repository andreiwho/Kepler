#include "FileUtils.h"
#include <fstream>

namespace Kepler
{
	std::future<TString> TFileUtils::ReadTextFileAsync(const TString& Path)
	{
		return Async([CopiedPath = Path] 
			{
				TString OutString;
#ifdef WIN32
				std::ifstream File{ CopiedPath, std::ios::ate | std::ios::binary };
#else
				std::ifstream File{ CopiedPath, std::ios::ate };
#endif
				if (!File.is_open())
				{
					CRASHMSG(fmt::format("Failed to read text file: {}", CopiedPath));
				}
				CHECK(File.is_open());
				usize Size = (usize)File.tellg();
				File.seekg(0);
				OutString.resize(Size + 1, 0);
				File.read(OutString.data(), Size);
				return OutString;
			});
	}
}