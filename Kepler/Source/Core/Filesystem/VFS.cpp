#include "VFS.h"
#include "Core/Core.h"

namespace Kepler
{
	TVirtualFileSystem* TVirtualFileSystem::Instance;

	TVirtualFileSystem::TVirtualFileSystem()
	{
		Instance = this;
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
		CHECK(!PathToResolve.starts_with('/'));
		CHECK(!TPath{ PathToResolve }.is_absolute());
		CHECK(PathToResolve.find(PathSeparationToken) != TString::npos);

		TString NewPath = PathToResolve;
		usize FirstSlash = PathToResolve.find_first_of(PathSeparationToken);
		if (FirstSlash != std::string::npos)
		{
			TString Alias = PathToResolve.substr(0, FirstSlash);
			if (PathAliases.Contains(Alias))
			{
				NewPath.replace(0, FirstSlash + strlen(PathSeparationToken), PathAliases[Alias] + '/');
				OutPath = std::move(NewPath);
				return true;
			}
		}
		return false;
	}

	void VFSRegisterPathAlias(const TString& Name, const TString& Path)
	{
		TVirtualFileSystem::Get()->RegisterVirtualPathAlias(Name, Path);
	}

	TString VFSResolvePath(const TString& Path)
	{
		TString OutString;
		CHECK(TVirtualFileSystem::Get()->ResolvePath(Path, OutString));
		return OutString;
	}

	TString VFSGetParentPath(const TString& Path)
	{
		if (Path.empty())
		{
			return {};
		}

		TPath Handle = Path;
		if (Handle.has_parent_path())
		{
			return Handle.parent_path().string();
		}
		return {};
	}

}