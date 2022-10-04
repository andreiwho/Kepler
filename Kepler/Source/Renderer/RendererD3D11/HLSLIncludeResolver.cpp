#include "HLSLIncludeResolver.h"
#include "Core/Filesystem/VFS.h"
#include "Core/Filesystem/FileUtils.h"
#include "Async/Async.h"

namespace ke
{

	THLSLIncludeResolverD3D11::THLSLIncludeResolverD3D11(const TString& InMyPath) 
		: MyPath(VFSGetParentPath(InMyPath))
	{
	}

	HRESULT THLSLIncludeResolverD3D11::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		const TString LoadedPath = std::invoke([&]
			{

				switch (IncludeType)
				{
				case D3D_INCLUDE_TYPE::D3D_INCLUDE_LOCAL:
				{
					return fmt::format("{}/{}", MyPath, pFileName);
					break;
				}
				case D3D_INCLUDE_TYPE::D3D10_INCLUDE_SYSTEM:
				{
					return VFSResolvePath(fmt::format("EngineShaders://{}", pFileName));
					break;
				}
				default:
					CRASH();
				}
			});

		if (!LoadedPath.empty())
		{
#ifdef WIN32
			std::ifstream Stream(LoadedPath, std::ios::ate | std::ios::binary);
#else
			std::ifstream Stream(LoadedPath, std::ios::ate);
#endif
			CHECK(Stream.is_open());
			usize FileSize = Stream.tellg();
			Stream.seekg(0);
			char* Memory = (char*)TMalloc::Get()->Allocate((usize)FileSize + 1);
			Memory[FileSize] = 0;
			Stream.read((char*)Memory, FileSize);
			*ppData = Memory;
			*pBytes = (UINT)FileSize + 1;
			return S_OK;
		}

		return E_FAIL;
	}

	HRESULT THLSLIncludeResolverD3D11::Close(LPCVOID Data)
	{
		TMalloc::Get()->Free(Data);
		return S_OK;
	}

}