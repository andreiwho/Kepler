#include "EntityHelper.h"

namespace Kepler
{

	TEntityHandle::TEntityHandle(TRef<TGameWorld> World, TGameEntityId EntityId)
		: GameWorld(World.Raw())
		, Id(EntityId)
	{
		GameWorld = World.Raw();
		if (GameWorld)
		{
			if(GameWorld->IsValidEntity(EntityId))
			{
				Entity = &World->GetEntityFromId(EntityId);
			}
		}
	}

}