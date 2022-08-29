#pragma once
#include "Core/Types.h"
#include "Core/Containers/ChaosMap.h"

namespace Kepler
{
	class TVirtualFileSystem
	{
	private:
		static TVirtualFileSystem* Instance;

	public:
		TVirtualFileSystem();

		static TVirtualFileSystem* Get() { return Instance; }

		inline TString GetCurrentWorkingDirectory() { return WorkingDirectory.string(); }

		void RegisterVirtualPathAlias(const TString& Name, const TString& Path);

		bool ResolvePath(const TString& PathToResolve, TString& OutPath);

	private:
		static constexpr const char* PathSeparationToken = "://";

		TPath WorkingDirectory = std::filesystem::current_path();

		TChaoticMap<TString, TString> PathAliases;
	};

	// Register a path to use it with the VFS
	// - Name: must be a sequence of characters, e.g. "Engine", "Game", "Lol"
	// - Path: must be path relative to the root directory of the project.
	void VFSRegisterPathAlias(const TString& Name, const TString& Path);

	// Important note: VFSResolvePath must happen on the lowest level of abstraction right before the function that loads the file.
	// Other functions higher on the callstack need to deal with unresolved paths.
	// Thus, resolving paths must happen right before any sort of interaction with the filesystem.
	TString VFSResolvePath(const TString& Path);
}