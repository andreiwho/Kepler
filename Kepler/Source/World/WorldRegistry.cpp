#include "WorldRegistry.h"

namespace ke
{
	
	ke::WorldRegistry* WorldRegistry::Instance;

	WorldRegistry::WorldRegistry()
	{
		Instance = this;
	}

	WorldRegistry::~WorldRegistry()
	{
		LoadedWorlds.Clear();
	}

	void WorldRegistry::DestroyWorld(RefPtr<TWorld> World)
	{
		auto Iter = LoadedWorlds.FindIterator([World](const auto& Other) { return Other.Raw() == World.Raw(); });
		if (LoadedWorlds.IsValidIterator(Iter))
		{
			LoadedWorlds.Remove(Iter);
		}
	}

}