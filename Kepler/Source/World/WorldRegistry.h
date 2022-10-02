#pragma once
#include "World.h"

namespace ke
{
	class TWorldRegistry
	{
		static TWorldRegistry* Instance;

	public:
		TWorldRegistry();
		~TWorldRegistry();

		template<typename T, typename... ARGS>
		TRef<T> CreateWorld(const TString& Name, ARGS&&... InArgs)
		{
			TRef<T> OutWorld = MakeRef(New<T>(Name, std::forward<ARGS>(InArgs)...));
			LoadedWorlds.EmplaceBack(OutWorld);
			return OutWorld;
		}

		void DestroyWorld(TRef<TWorld> World);

		static TWorldRegistry* Get() { return Instance; }

	private:
		TDynArray<TRef<TWorld>> LoadedWorlds;
	};
}