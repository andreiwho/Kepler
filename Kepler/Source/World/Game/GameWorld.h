#pragma once
#include "World/World.h"
#include "GameEntityId.h"
#include "Components/EntityComponent.h"
#include "World/Scripting/NativeScriptComponent.h"

#include <entt/entt.hpp>
#include <functional>

namespace ke
{
	class TGameEntity;

	template<typename T>
	concept entity_component_typename = std::is_base_of_v<EntityComponent, T>;

	struct NativeComponentAccessors
	{
		std::function<void()> OnInit;
		std::function<void(float)> OnUpdate;
		std::function<void()> OnDestroy;
		std::function<void(class GameWorld*)> OnCopy;
	};

	struct NativeComponentInfo
	{
		bool bShouldUpdate = true;
	};

	class GameWorld : public TWorld
	{
		friend class NativeScriptContainerComponent;
	public:
		GameWorld(const TString& InName);

		~GameWorld();

		GameEntityId CreateEntity(const TString& Name);
		GameEntityId CreateCamera(const TString& Name, float Fov = 45.0f, float Width = 0, float Height = 0, float Near = 0.1f, float Far = 100.0f);
		TGameEntity& GetEntityFromId(GameEntityId Id);

		void DestroyEntity(GameEntityId Entity);

		TString GetEntityName(GameEntityId Entity);

		id64 GetEntityUUID(GameEntityId Entity) const;

		bool IsValidEntity(GameEntityId Id) const;

		template<typename T>
		void SetupNativeComponent()
		{
			NativeComponentAccessors accessors;
			const u64 typeHash = typeid(T).hash_code();
			if (m_NativeComponentInfos.Contains(typeHash))
			{
				return;
			}

			if constexpr (HasInitFunction<T>::value)
			{
				accessors.OnInit = [world = this]()
				{
					world->GetComponentView<T>().each(
						[](auto, T& comp)
						{
							comp.Init();
						});
				};
			}
			else
			{
				accessors.OnInit = []() {};
			}

			if constexpr (HasUpdateFunction<T>::value)
			{
				accessors.OnUpdate = [world = this](float deltaTime)
				{
					world->GetComponentView<T>().each(
						[deltaTime](auto, T& comp)
						{
							comp.Update(deltaTime);
						});
				};
			}
			else
			{
				accessors.OnUpdate = [](float deltaTime) {};
			}

			if constexpr (HasDestroyingFunction<T>::value)
			{
				accessors.OnDestroy = [world = this]()
				{
					world->GetComponentView<T>().each(
						[](auto, T& comp)
						{
							comp.Destroying();
						});
				};
			}
			else
			{
				accessors.OnDestroy = []() {};
			}



			m_NativeAccessors.AppendBack(accessors);
			m_NativeComponentInfos.Insert(typeHash, {});
		}

		template<entity_component_typename T, typename ... ARGS>
		T& AddComponent(GameEntityId entity, ARGS&&... InArgs)
		{
			if constexpr (std::is_base_of_v<NativeScriptComponent, T>)
			{
				T& component = EntityRegistry.emplace<T>(entity);
				SetupNativeComponent<T>();
				component.SetOwner(entity);
				component.SetWorld(this);
				return component;
			}

			T& component = EntityRegistry.emplace<T>(entity.Entity, std::forward<ARGS>(InArgs)...);
			component.SetOwner(entity);
			component.SetWorld(this);
			return component;
		}

		template<entity_component_typename T>
		T& GetOrAddComponent(GameEntityId entity)
		{
			if (!HasComponent<T>(entity))
			{
				return AddComponent<T>(entity);
			}
			return GetComponent<T>(entity);
		}

		template<entity_component_typename T>
		T& GetComponent(GameEntityId Entity)
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		const T& GetComponent(GameEntityId Entity) const
		{
			return EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		inline bool HasComponent(GameEntityId Entity) const
		{
			return EntityRegistry.any_of<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		void RemoveComponent(GameEntityId Entity)
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

		void SetMainCamera(GameEntityId Camera);

		inline GameEntityId GetMainCamera() const { return MainCamera; }

		// Check functons
		bool IsCamera(GameEntityId Entity) const;
		bool IsLight(GameEntityId Entity) const;

	private:
		void FlushPendingDestroys();

		entt::registry EntityRegistry;

		Array<entt::entity> PendingDestroyEntities;
		Array<NativeComponentAccessors> m_NativeAccessors;
		Map<u64, NativeComponentInfo> m_NativeComponentInfos;

		GameEntityId MainCamera{};
	};
}