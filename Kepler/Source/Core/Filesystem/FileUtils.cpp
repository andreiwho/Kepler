#include "FileUtils.h"
#include "Core/Filesystem/VFS.h"
#include <fstream>

namespace ke
{
	std::future<TString> TFileUtils::ReadTextFileAsync(const TString& path)
	{
		return Async([CopiedPath = VFSResolvePath(path)]
			{
				TString outStr;
#ifdef WIN32
				std::ifstream stream{ CopiedPath, std::ios::ate | std::ios::binary };
#else
				std::ifstream stream{ CopiedPath, std::ios::ate };
#endif
				if (!stream.is_open())
				{
					CRASHMSG(fmt::format("Failed to read text file: {}", CopiedPath));
				}
				CHECK(stream.is_open());
				usize Size = (usize)stream.tellg();
				stream.seekg(0);
				outStr.resize(Size + 1, 0);
				stream.read(outStr.data(), Size);
				return outStr;
			});
	}

	std::future<TDynArray<u8>> TFileUtils::ReadBinaryFileAsync(const TString& path)
	{
		return Async([CopiedPath = VFSResolvePath(path)]
			{
				TDynArray<u8> outData;
#ifdef WIN32
				std::ifstream stream{ CopiedPath, std::ios::ate | std::ios::binary };
#else
				std::ifstream stream{ CopiedPath, std::ios::ate };
#endif
				if (!stream.is_open())
				{
					CRASHMSG(fmt::format("Failed to read binary file: {}", CopiedPath));
				}
				CHECK(stream.is_open());
				usize Size = (usize)stream.tellg();
				stream.seekg(0);
				outData.Resize(Size + 1);
				stream.read((char*)outData.GetData(), Size);
				outData[Size] = 0;
				return outData;
			});
	}

	bool TFileUtils::PathExists(const TString& path)
	{
		return std::filesystem::exists(VFSResolvePath(path));
	}

}