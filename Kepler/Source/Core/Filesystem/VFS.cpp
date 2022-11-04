#include "VFS.h"
#include "Core/Core.h"

namespace ke
{
	TVirtualFileSystem* TVirtualFileSystem::Instance;

	TVirtualFileSystem::TVirtualFileSystem()
	{
		Instance = this;
	}

	void TVirtualFileSystem::RegisterVirtualPathAlias(const String& name, const String& path)
	{
		CHECK(!name.starts_with(' '));
		CHECK(!path.starts_with(' '));
		CHECK(!m_Aliases.Contains(name));

		String newPath = std::filesystem::relative(path, GetCurrentWorkingDirectory()).string();
		std::replace(newPath.begin(), newPath.end(), '\\', '/');
		m_Aliases.Insert(name, newPath);
	}

	bool TVirtualFileSystem::ResolvePath(const String& path, String& outPath)
	{
		CHECK(!path.starts_with(' '));
		CHECK(!path.starts_with('/'));
		CHECK(!TPath{ path }.is_absolute());
		CHECK(path.find(m_VFSToken) != String::npos);

		String newPath = path;
		usize firstSlashIndex = path.find_first_of(m_VFSToken);
		if (firstSlashIndex != std::string::npos)
		{
			String alias = path.substr(0, firstSlashIndex);
			if (m_Aliases.Contains(alias))
			{
				newPath.replace(0, firstSlashIndex + strlen(m_VFSToken), m_Aliases[alias] + '/');
				outPath = std::move(newPath);
				return true;
			}
		}
		return false;
	}

	void VFSRegisterPathAlias(const String& name, const String& path)
	{
		TVirtualFileSystem::Get()->RegisterVirtualPathAlias(name, path);
	}

	String VFSResolvePath(const String& path)
	{
		String outStr;
		CHECK(TVirtualFileSystem::Get()->ResolvePath(path, outStr));
		return outStr;
	}

	String VFSGetParentPath(const String& path)
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