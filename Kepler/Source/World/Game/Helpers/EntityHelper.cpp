#include "EntityHelper.h"

namespace ke
{

	TEntityHandle::TEntityHandle(RefPtr<GameWorld> world, TGameEntityId id)
		: m_GameWorld(world.Raw())
		, m_Id(id)
	{
		m_GameWorld = world.Raw();
		if (m_GameWorld)
		{
			if(m_GameWorld->IsValidEntity(id))
			{
				m_Entity = &m_GameWorld->GetEntityFromId(id);
			}
		}
	}

}