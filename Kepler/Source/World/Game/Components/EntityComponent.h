#pragma once
#include "../GameEntityId.h"
#include "EntityComponent.gen.h"

namespace ke
{
	class GameWorld;

	namespace detail
	{
	}

	reflected class EntityComponent : public Object
	{
		reflected_body();
	public:
		virtual ~EntityComponent() = default;

		// Called after the component is constructed and setup
		virtual void OnAttach() {}
		void SetOwner(GameEntityId ownerId);
		void SetWorld(GameWorld* pWorld);

		inline GameEntityId GetOwner() const { return m_OwnerId; }
		inline GameWorld* GetWorld() { return m_World; }

		template<typename T>
		void RequireComponent()
		{
			CHECK(m_World);
			if (auto pClass = GetReflectedClass<T>())
			{
				RequireComponent(pClass->GetClassId());
			}
		}

		void RequireComponent(ClassId id);

	private:
		GameEntityId m_OwnerId{};
		GameWorld* m_World;
	};
}