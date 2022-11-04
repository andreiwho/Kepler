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

		template<typename T, typename... ARGS>
		RefPtr<T> CreateWorldAtIndex(usize index, const String& Name, ARGS&&... InArgs)
		{
			RefPtr<T> OutWorld = MakeRef(New<T>(Name, std::forward<ARGS>(InArgs)...));
			if (LoadedWorlds.GetLength() <= index)
			{
				LoadedWorlds.Resize(index + 1);
			}
			LoadedWorlds[index] = OutWorld;
			return OutWorld;
		}

		void DestroyWorld(RefPtr<TWorld> World);

		static WorldRegistry* Get() { return Instance; }

		RefPtr<TWorld> GetWorldAt(u32 index)
		{
			if (LoadedWorlds.GetLength() <= index)
			{
				return nullptr;
			}
			return LoadedWorlds[index];
		}

	private:
		Array<RefPtr<TWorld>> LoadedWorlds;
	};
}