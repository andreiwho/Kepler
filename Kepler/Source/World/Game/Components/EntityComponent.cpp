#include "EntityComponent.h"

namespace ke
{
	void EntityComponent::SetOwner(TGameEntityId ownerId)
	{
		m_OwnerId = ownerId;
	}

	void EntityComponent::SetWorld(GameWorld* pWorld)
	{
		m_World = pWorld;
	}

}