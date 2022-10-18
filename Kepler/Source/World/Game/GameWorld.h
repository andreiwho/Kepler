#pragma once
#include "World/World.h"
#include "GameEntityId.h"
#include "Components/EntityComponent.h"

#include <entt/entt.hpp>

namespace ke
{
	class TGameEntity;

	template<typename T>
	concept entity_component_typename = std::is_base_of_v<EntityComponent, T>;

	class GameWorld : public TWorld
	{
	public:
		GameWorld(const TString& InName);

		~GameWorld();

		TGameEntityId CreateEntity(const TString& Name);
		TGameEntityId CreateCamera(const TString& Name, float Fov = 45.0f, float Width = 0, float Height = 0, float Near = 0.1f, float Far = 100.0f);
		TGameEntity& GetEntityFromId(TGameEntityId Id);

		void DestroyEntity(TGameEntityId Entity);

		TString GetEntityName(TGameEntityId Entity);

		id64 GetEntityUUID(TGameEntityId Entity) const;

		bool IsValidEntity(TGameEntityId Id) const;

		template<entity_component_typename T, typename ... ARGS>
		T& AddComponent(TGameEntityId Entity, ARGS&&... InArgs)
		{
			T& component = EntityRegistry.emplace<T>(Entity.Entity, std::forward<ARGS>(InArgs)...);
			component.SetOwner(Entity);
			component.SetWorld(this);
			return component;
		}

		template<entity_component_typename T>
		T& GetComponent(TGameEntityId Entity)
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		const T& GetComponent(TGameEntityId Entity) const
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		inline bool HasComponent(TGameEntityId Entity) const
		{
			return EntityRegistry.any_of<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		void RemoveComponent(TGameEntityId Entity) 
		{
			EntityRegistry.remove<T>(Entity.Entity);
		}

		template<entity_component_typename ... Ts>
		inline decltype(auto) GetComponentView() 
		{
			if constexpr (sizeof...(Ts) > 1)
			{
				return EntityRegistry.group<Ts...>();
			}
			else
			{
				return EntityRegistry.view<Ts...>();
			}
		}

		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind) override;

		void SetMainCamera(TGameEntityId Camera);

		inline TGameEntityId GetMainCamera() const { return MainCamera; }

		// Check functons
		bool IsCamera(TGameEntityId Entity) const;
		bool IsLight(TGameEntityId Entity) const;

	private:
		void FlushPendingDestroys();

		entt::registry EntityRegistry;

		Array<entt::entity> PendingDestroyEntities;

		TGameEntityId MainCamera{};
	};
}