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
		TPath WorkingDirectory = std::filesystem::current_path();

		TChaoticMap<TString, TString> PathAliases;
	};

	void VFSRegisterPathAlias(const TString& Name, const TString& Path);
	TString VFSResolvePath(const TString& Path);
}