#pragma once
#include "World/World.h"
#include "GameEntityId.h"

#include <entt/entt.hpp>

namespace ke
{
	class TGameEntity;

	class TGameWorld : public TWorld
	{
	public:
		TGameWorld(const TString& InName);

		~TGameWorld();

		TGameEntityId CreateEntity(const TString& Name);
		TGameEntityId CreateCamera(const TString& Name, float Fov = 45.0f, float Width = 0, float Height = 0, float Near = 0.1f, float Far = 100.0f);
		TGameEntity& GetEntityFromId(TGameEntityId Id);

		void DestroyEntity(TGameEntityId Entity);

		TString GetEntityName(TGameEntityId Entity);

		id64 GetEntityUUID(TGameEntityId Entity) const;

		bool IsValidEntity(TGameEntityId Id) const;

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
		inline bool HasComponent(TGameEntityId Entity) const
		{
			return EntityRegistry.any_of<T>(Entity.Entity);
		}

		template<typename T> 
		void RemoveComponent(TGameEntityId Entity) 
		{
			EntityRegistry.remove<T>(Entity.Entity);
		}

		template<typename ... Ts>
		inline decltype(auto) GetComponentView()
		{
			return EntityRegistry.view<Ts...>();
		}

		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind) override;

		void SetMainCamera(TGameEntityId Camera);

		inline TGameEntityId GetMainCamera() const { return MainCamera; }

		// Check functons
		bool IsCamera(TGameEntityId Entity) const;

	private:
		void FlushPendingDestroys();

		entt::registry EntityRegistry;

		TDynArray<entt::entity> PendingDestroyEntities;

		TGameEntityId MainCamera{};
	};
}