#include "WorldRegistry.h"

namespace Kepler
{
	
	Kepler::TWorldRegistry* TWorldRegistry::Instance;

	TWorldRegistry::TWorldRegistry()
	{
		Instance = this;
	}

	TWorldRegistry::~TWorldRegistry()
	{
		LoadedWorlds.Clear();
	}

	void TWorldRegistry::DestroyWorld(TRef<TWorld> World)
	{
		auto Iter = LoadedWorlds.FindIterator([World](const auto& Other) { return Other.Raw() == World.Raw(); });
		if (LoadedWorlds.IsValidIterator(Iter))
		{
			LoadedWorlds.Remove(Iter);
		}
	}

}