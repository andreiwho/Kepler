#include "EntityComponent.h"

namespace ke
{
	void EntityComponent::SetOwner(GameEntityId ownerId)
	{
		m_OwnerId = ownerId;
	}

	void EntityComponent::SetWorld(GameWorld* pWorld)
	{
		m_World = pWorld;
	}

}