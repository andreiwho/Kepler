#include "EntityComponent.h"
#include "../GameWorld.h"

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

	void EntityComponent::RequireComponent(ClassId id)
	{
		if (m_World->HasComponent(id, m_OwnerId))
		{
			return;
		}

		m_World->AddComponentByTypeHash(m_OwnerId, id);
	}
}