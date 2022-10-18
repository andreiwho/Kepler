#pragma once
#include "../GameEntityId.h"

namespace ke
{
	class GameWorld;

	class EntityComponent
	{
	public:
		virtual ~EntityComponent() = default;

		void SetOwner(TGameEntityId ownerId);
		void SetWorld(GameWorld* pWorld);

		inline TGameEntityId GetOwner() const { return m_OwnerId; }
		inline GameWorld* GetWorld() { return m_World; }

		virtual void Init() {}
		virtual void Update(float deltaTime) {}

	private:
		TGameEntityId m_OwnerId{};
		GameWorld* m_World;
	};
}