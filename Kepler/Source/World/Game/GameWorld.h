#pragma once
#include "World/World.h"
#include "GameEntityId.h"

#include <entt/entt.hpp>

namespace Kepler
{
	class TGameEntity;

	class TGameWorld : public TWorld
	{
	public:
		TGameWorld(const TString& InName);

		~TGameWorld();

		TGameEntityId CreateEntity(const TString& Name);
		TGameEntity& GetEntityFromId(TGameEntityId Id);

		void DestroyEntity(TGameEntityId Entity);

		TString GetEntityName(TGameEntityId Entity);

		id64 GetEntityUUID(TGameEntityId Entity) const;

		template<typename T, typename ... ARGS>
		T& AddComponent(TGameEntityId Entity, ARGS&&... InArgs)
		{
			return EntityRegistry.emplace<T>(Entity.Entity, std::forward<ARGS>(InArgs)...);
		}

		template<typename T>
		T& GetComponent(TGameEntityId Entity)
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}

		template<typename T>
		const T& GetComponent(TGameEntityId Entity) const
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}


		template<typename T> 
		void RemoveComponent(TGameEntityId Entity) 
		{
			EntityRegistry.remove<T>(Entity.Entity);
		}

		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind) override;

	private:
		void FlushPendingDestroys();

		entt::registry EntityRegistry;

		TDynArray<entt::entity> PendingDestroyEntities;
	};
}