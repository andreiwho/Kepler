#pragma once
#include "World.h"

namespace ke
{
	class WorldRegistry
	{
		static WorldRegistry* Instance;

	public:
		WorldRegistry();
		~WorldRegistry();

		template<typename T, typename... ARGS>
		RefPtr<T> CreateWorld(const String& Name, ARGS&&... InArgs)
		{
			RefPtr<T> OutWorld = MakeRef(New<T>(Name, std::forward<ARGS>(InArgs)...));
			LoadedWorlds.EmplaceBack(OutWorld);
			return OutWorld;
		}

		void DestroyWorld(RefPtr<TWorld> World);

		static WorldRegistry* Get() { return Instance; }

	private:
		Array<RefPtr<TWorld>> LoadedWorlds;
	};
}