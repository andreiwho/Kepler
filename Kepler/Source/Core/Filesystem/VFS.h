#pragma once
#include "Core/Types.h"
#include "Core/Containers/ChaosMap.h"

namespace ke
{
	class TVirtualFileSystem
	{
	private:
		static TVirtualFileSystem* Instance;

	public:
		TVirtualFileSystem();

		static TVirtualFileSystem* Get() { return Instance; }
		inline String GetCurrentWorkingDirectory() { return m_Cwd.string(); }
		void RegisterVirtualPathAlias(const String& name, const String& path);
		bool ResolvePath(const String& PathToResolve, String& OutPath);
		const Map<String, String>& GetPathAliases() const& { return m_Aliases; }

	private:
		static constexpr const char* m_VFSToken = "://";
		TPath m_Cwd = std::filesystem::current_path();
		Map<String, String> m_Aliases;
	};

	// Register a path to use it with the VFS
	// - Name: must be a sequence of characters, e.g. "Engine", "Game", "Lol"
	// - Path: must be path relative to the root directory of the project.
	void VFSRegisterPathAlias(const String& name, const String& path);

	// Important note: VFSResolvePath must happen on the lowest level of abstraction right before the function that loads the file.
	// Other functions higher on the callstack need to deal with unresolved paths.
	// Thus, resolving paths must happen right before any sort of interaction with the filesystem.
	String VFSResolvePath(const String& path);
	
	String VFSGetParentPath(const String& path);
}