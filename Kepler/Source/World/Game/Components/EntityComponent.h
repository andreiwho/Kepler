#pragma once
#include "../GameEntityId.h"
#include "EntityComponent.gen.h"

namespace ke
{
	class GameWorld;

	reflected class EntityComponent : public Object
	{
		reflection_info();
	public:
		virtual ~EntityComponent() = default;

		void SetOwner(GameEntityId ownerId);
		void SetWorld(GameWorld* pWorld);

		inline GameEntityId GetOwner() const { return m_OwnerId; }
		inline GameWorld* GetWorld() { return m_World; }

	private:
		GameEntityId m_OwnerId{};
		GameWorld* m_World;
	};
}