#include "EntityHelper.h"

namespace ke
{

	EntityHandle::EntityHandle(GameWorld* world, GameEntityId id)
		: m_GameWorld(world)
		, m_Id(id)
	{
		m_GameWorld = world;
		if (m_GameWorld)
		{
			if(m_GameWorld->IsValidEntity(id))
			{
				m_Entity = &m_GameWorld->GetEntityFromId(id);
			}
		}
	}

}