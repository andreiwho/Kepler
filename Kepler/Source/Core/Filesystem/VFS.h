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
		inline TString GetCurrentWorkingDirectory() { return m_Cwd.string(); }
		void RegisterVirtualPathAlias(const TString& name, const TString& path);
		bool ResolvePath(const TString& PathToResolve, TString& OutPath);
		const Map<TString, TString>& GetPathAliases() const& { return m_Aliases; }

	private:
		static constexpr const char* m_VFSToken = "://";
		TPath m_Cwd = std::filesystem::current_path();
		Map<TString, TString> m_Aliases;
	};

	// Register a path to use it with the VFS
	// - Name: must be a sequence of characters, e.g. "Engine", "Game", "Lol"
	// - Path: must be path relative to the root directory of the project.
	void VFSRegisterPathAlias(const TString& name, const TString& path);

	// Important note: VFSResolvePath must happen on the lowest level of abstraction right before the function that loads the file.
	// Other functions higher on the callstack need to deal with unresolved paths.
	// Thus, resolving paths must happen right before any sort of interaction with the filesystem.
	TString VFSResolvePath(const TString& path);
	
	TString VFSGetParentPath(const TString& path);
}