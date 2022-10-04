#include "VFS.h"
#include "Core/Core.h"

namespace ke
{
	TVirtualFileSystem* TVirtualFileSystem::Instance;

	TVirtualFileSystem::TVirtualFileSystem()
	{
		Instance = this;
	}

	void TVirtualFileSystem::RegisterVirtualPathAlias(const TString& name, const TString& path)
	{
		CHECK(!name.starts_with(' '));
		CHECK(!path.starts_with(' '));
		CHECK(!m_Aliases.Contains(name));

		TString newPath = std::filesystem::relative(path, GetCurrentWorkingDirectory()).string();
		std::replace(newPath.begin(), newPath.end(), '\\', '/');
		m_Aliases.Insert(name, newPath);
	}

	bool TVirtualFileSystem::ResolvePath(const TString& path, TString& outPath)
	{
		CHECK(!path.starts_with(' '));
		CHECK(!path.starts_with('/'));
		CHECK(!TPath{ path }.is_absolute());
		CHECK(path.find(m_VFSToken) != TString::npos);

		TString newPath = path;
		usize firstSlashIndex = path.find_first_of(m_VFSToken);
		if (firstSlashIndex != std::string::npos)
		{
			TString alias = path.substr(0, firstSlashIndex);
			if (m_Aliases.Contains(alias))
			{
				newPath.replace(0, firstSlashIndex + strlen(m_VFSToken), m_Aliases[alias] + '/');
				outPath = std::move(newPath);
				return true;
			}
		}
		return false;
	}

	void VFSRegisterPathAlias(const TString& name, const TString& path)
	{
		TVirtualFileSystem::Get()->RegisterVirtualPathAlias(name, path);
	}

	TString VFSResolvePath(const TString& path)
	{
		TString outStr;
		CHECK(TVirtualFileSystem::Get()->ResolvePath(path, outStr));
		return outStr;
	}

	TString VFSGetParentPath(const TString& path)
	{
		if (path.empty())
		{
			return {};
		}

		TPath handle = path;
		if (handle.has_parent_path())
		{
			return handle.parent_path().string();
		}
		return {};
	}

}