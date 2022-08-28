#include "VFS.h"
#include "Core/Core.h"

namespace Kepler
{
	TVirtualFileSystem* TVirtualFileSystem::Instance;

	TVirtualFileSystem::TVirtualFileSystem()
	{
		Instance = this;

		RegisterVirtualPathAlias("Engine", GetCurrentWorkingDirectory() + "/Kepler/Assets");
		RegisterVirtualPathAlias("EngineShaders", GetCurrentWorkingDirectory() + "/Kepler/Shaders");
	}

	void TVirtualFileSystem::RegisterVirtualPathAlias(const TString& Name, const TString& Path)
	{
		CHECK(!Name.starts_with(' '));
		CHECK(!Path.starts_with(' '));
		CHECK(!PathAliases.Contains(Name));

		TString NewPath = std::filesystem::relative(Path, GetCurrentWorkingDirectory()).string();
		std::replace(NewPath.begin(), NewPath.end(), '\\', '/');
		
		PathAliases.Insert(Name, NewPath);
	}

	bool TVirtualFileSystem::ResolvePath(const TString& PathToResolve, TString& OutPath)
	{
		CHECK(!PathToResolve.starts_with(' '));
		CHECK(!TPath{ PathToResolve }.is_absolute());

		TString NewPath = PathToResolve;
		usize FirstSlash = PathToResolve.find_first_of('/');
		if (FirstSlash != std::string::npos)
		{
			TString Alias = PathToResolve.substr(0, FirstSlash);
			if (PathAliases.Contains(Alias))
			{
				NewPath.replace(0, FirstSlash, PathAliases[Alias]);
				OutPath = std::move(NewPath);
				return true;
			}
			else
			{
				OutPath = fmt::format("{}/{}", GetCurrentWorkingDirectory(), PathToResolve);
				std::replace(OutPath.begin(), OutPath.end(), '\\', '/');
			}
		}
		return false;
	}

	void VFSRegisterPathAlias(const TString& Name, const TString& Path)
	{
		TVirtualFileSystem::Get()->RegisterVirtualPathAlias(Name, TVirtualFileSystem::Get()->GetCurrentWorkingDirectory() + "/" + Path);
	}

	TString VFSResolvePath(const TString& Path)
	{
		TString OutString;
		TVirtualFileSystem::Get()->ResolvePath(Path, OutString);
		return OutString;
	}

}