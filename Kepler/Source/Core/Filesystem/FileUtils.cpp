#include "FileUtils.h"
#include "Core/Filesystem/VFS.h"
#include <fstream>

namespace ke
{
	std::future<TString> TFileUtils::ReadTextFileAsync(const TString& Path)
	{
		return Async([CopiedPath = VFSResolvePath(Path)]
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

	std::future<TDynArray<u8>> TFileUtils::ReadBinaryFileAsync(const TString& Path)
	{
		return Async([CopiedPath = VFSResolvePath(Path)]
			{
				TDynArray<u8> OutData;
#ifdef WIN32
				std::ifstream File{ CopiedPath, std::ios::ate | std::ios::binary };
#else
				std::ifstream File{ CopiedPath, std::ios::ate };
#endif
				if (!File.is_open())
				{
					CRASHMSG(fmt::format("Failed to read binary file: {}", CopiedPath));
				}
				CHECK(File.is_open());
				usize Size = (usize)File.tellg();
				File.seekg(0);
				OutData.Resize(Size + 1);
				File.read((char*)OutData.GetData(), Size);
				OutData[Size] = 0;
				return OutData;
			});
	}

	bool TFileUtils::PathExists(const TString& Path)
	{
		return std::filesystem::exists(VFSResolvePath(Path));
	}

}