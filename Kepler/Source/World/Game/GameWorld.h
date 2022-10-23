#pragma once
#include "World/World.h"
#include "GameEntityId.h"
#include "Components/EntityComponent.h"
#include "World/Scripting/NativeScriptComponent.h"

#include <entt/entt.hpp>
#include <functional>
#include "../Scripting/NativeScriptContainer.h"

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
		std::function<NativeScriptComponent* (GameEntityId id)> GetComponent;
		std::function<void(class GameWorld*)> OnCopy;
	};

	struct NativeComponentInfo
	{
		u64 AccessorId = 0;
	};

	class GameWorld : public TWorld
	{
		friend class NativeScriptContainerComponent;
	public:
		GameWorld(const String& InName);

		~GameWorld();

		GameEntityId CreateEntity(const String& Name);
		GameEntityId CreateCamera(const String& Name, float Fov = 45.0f, float Width = 0, float Height = 0, float Near = 0.1f, float Far = 100.0f);
		TGameEntity& GetEntityFromId(GameEntityId Id);

		void DestroyEntity(GameEntityId Entity);

		String GetEntityName(GameEntityId Entity);

		id64 GetEntityUUID(GameEntityId Entity) const;

		bool IsValidEntity(GameEntityId Id) const;

		template<typename T>
		void SetupNativeComponent()
		{
			RefPtr<ReflectedClass> pComponentClass = ReflectionDatabase::Get()->GetClass<T>();
			CHECKMSG(pComponentClass, "Native component classes must have 'reflected' specifier.");

			NativeComponentAccessors accessors;
			if (m_NativeComponentInfos.Contains(pComponentClass->GetClassId()))
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

			accessors.GetComponent = [world = this](GameEntityId gameEntityId)
			{
				if (world->HasComponent<T>(gameEntityId))
				{
					return &world->GetComponent<T>(gameEntityId);
				}
				return (T*)nullptr;
			};


			usize accessorId = m_NativeAccessors.AppendBack(accessors);
			NativeComponentInfo info;
			info.AccessorId = accessorId;

			m_NativeComponentInfos.Insert(pComponentClass->GetClassId(), info);
		}

		template<entity_component_typename T, typename ... ARGS>
		T& AddComponent(GameEntityId entity, ARGS&&... InArgs)
		{
			if constexpr (std::is_base_of_v<NativeScriptComponent, T>)
			{
				NativeScriptContainerComponent& container = GetOrAddComponent<NativeScriptContainerComponent>(entity);
				container.AddComponent<T>();

				T& component = m_EntityRegistry.emplace<T>(entity);
				SetupNativeComponent<T>();
				component.SetOwner(entity);
				component.SetWorld(this);
				return component;
			}

			T& component = m_EntityRegistry.emplace<T>(entity.Entity, std::forward<ARGS>(InArgs)...);
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
			return m_EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		const T& GetComponent(GameEntityId Entity) const
		{
			return m_EntityRegistry.get<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		inline bool HasComponent(GameEntityId Entity) const
		{
			return m_EntityRegistry.any_of<T>(Entity.Entity);
		}

		template<entity_component_typename T>
		void RemoveComponent(GameEntityId Entity)
		{
			m_EntityRegistry.remove<T>(Entity.Entity);
		}

		template<entity_component_typename ... Ts>
		inline decltype(auto) GetComponentView()
		{
			if constexpr (sizeof...(Ts) > 1)
			{
				return m_EntityRegistry.group<Ts...>();
			}
			else
			{
				return m_EntityRegistry.view<Ts...>();
			}
		}

		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind) override;

		void SetMainCamera(GameEntityId Camera);

		inline GameEntityId GetMainCamera() const { return m_MainCamera; }

		inline usize GetNumEntities() const
		{
			return m_EntityRegistry.alive();
		}

		// The Func should be void(GameEntityId)
		template<typename Func>
		void ForEachEntity(Func func)
		{
			m_EntityRegistry.each(func);
		}
 		
		// Check functons
		bool IsCamera(GameEntityId Entity) const;
		bool IsLight(GameEntityId Entity) const;

		NativeScriptComponent* GetNativeComponentById(id64 componentId, GameEntityId entityId)
		{
			if (!m_NativeComponentInfos.Contains(componentId))
			{
				return nullptr;
			}

			auto accessorId = m_NativeComponentInfos[componentId].AccessorId;
			return m_NativeAccessors[accessorId].GetComponent(entityId);
		}

	private:
		void FlushPendingDestroys();

		entt::registry m_EntityRegistry;

		Array<entt::entity> m_PendingDestroyEntities;
		Array<NativeComponentAccessors> m_NativeAccessors;
		Map<id64, NativeComponentInfo> m_NativeComponentInfos;

		GameEntityId m_MainCamera{};
	};
}