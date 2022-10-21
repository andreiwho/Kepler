#pragma once
#include "../GameEntityId.h"
#include "EntityComponent.gen.h"

namespace ke
{
	class GameWorld;

	reflected class EntityComponent
	{
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

	reflected class TestComponent : public EntityComponent
	{
	public:
		reflected float SomeValue;
		reflected bool SomeOtherValue;
	};
}