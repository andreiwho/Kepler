#pragma once
#include "World/World.h"
#include "GameEntityId.h"
#include "Components/EntityComponent.h"
#include "World/Scripting/NativeScriptComponent.h"

#include <entt/entt.hpp>
#include <functional>
#include "../Scripting/NativeComponentContainer.h"
#include "GameWorld.gen.h"

namespace ke
{
	class TGameEntity;

	template<typename T>
	concept entity_component_typename = std::is_base_of_v<EntityComponent, T>;

	struct NativeComponentAccessors
	{
		std::function<void(GameWorld*)> OnInit;
		std::function<void(GameWorld*, float)> OnUpdate;
		std::function<void(GameWorld*)> OnDestroy;
		std::function<EntityComponent* (GameWorld*,GameEntityId id)> GetComponent;
		std::function<EntityComponent* (GameWorld*,GameEntityId id)> ConstructComponent;
		std::function<void(class GameWorld*)> OnCopy;
	};

	struct NativeComponentInfo
	{
		u64 AccessorId = 0;
	};

	reflected class GameWorld : public TWorld
	{
		reflected_body();
		friend class NativeComponentContainer;
	public:
		GameWorld() = default;
		GameWorld(const String& InName);

		~GameWorld();

		GameEntityId CreateEntity(const String& Name);
		GameEntityId CreateCamera(const String& Name, float Fov = 45.0f, float Width = 0, float Height = 0, float Near = 0.1f, float Far = 100.0f);
		GameEntityId CreateEntityDeferred();
		void FinishCreatingEntity(GameEntityId entity);
		TGameEntity& GetEntityFromId(GameEntityId Id);
		GameEntityId GetEntityByUUID(UUID id);

		void DestroyEntity(GameEntityId Entity);

		String GetEntityName(GameEntityId Entity);

		UUID GetEntityUUID(GameEntityId Entity) const;

		bool IsValidEntity(GameEntityId Id) const;

		template<typename T>
		static void SetupNativeComponent()
		{
			RefPtr<ReflectedClass> pComponentClass = ReflectionDatabase::Get()->GetClass<T>();
			CHECKMSG(pComponentClass, "Native component classes must have 'reflected' specifier.");

			NativeComponentAccessors accessors;
			if (m_StaticState->m_ComponentInfos.Contains(pComponentClass->GetClassId()))
			{
				return;
			}

			if constexpr (HasInitFunction<T>::value)
			{
				accessors.OnInit = [](GameWorld* world)
				{
					world->GetComponentView<T>().each(
						[](auto, T& comp)
						{
							comp.Init();
						});
				};
			}

			if constexpr (HasUpdateFunction<T>::value)
			{
				accessors.OnUpdate = [](GameWorld* world, float deltaTime)
				{
					world->GetComponentView<T>().each(
						[deltaTime](auto, T& comp)
						{
							comp.Update(deltaTime);
						});
				};
			}

			if constexpr (HasDestroyingFunction<T>::value)
			{
				accessors.OnDestroy = [](GameWorld* world)
				{
					world->GetComponentView<T>().each(
						[](auto, T& comp)
						{
							comp.Destroying();
						});
				};
			}

			accessors.ConstructComponent = [](GameWorld* world, GameEntityId gameEntityId)
			{
				return &world->AddComponent<T>(gameEntityId);
			};

			accessors.GetComponent = [](GameWorld* world, GameEntityId gameEntityId)
			{
				if (world->HasComponent<T>(gameEntityId))
				{
					return &world->GetComponent<T>(gameEntityId);
				}
				return (T*)nullptr;
			};


			usize accessorId = m_StaticState->m_NativeAccessors.AppendBack(accessors);
			NativeComponentInfo info;
			info.AccessorId = accessorId;

			m_StaticState->m_ComponentInfos.Insert(pComponentClass->GetClassId(), info);
		}

		template<entity_component_typename T, typename ... ARGS>
		T& AddComponent(GameEntityId entity, ARGS&&... InArgs)
		{
			if constexpr (std::is_same_v<NativeComponentContainer, T>)
			{
				return m_EntityRegistry.emplace<NativeComponentContainer>(entity.Entity);
			}
			else
			{
				NativeComponentContainer& container = GetOrAddComponent<NativeComponentContainer>(entity);
				container.AddComponent<T>();

				T& component = m_EntityRegistry.emplace<T>(entity.Entity, std::forward<ARGS>(InArgs)...);
				SetupNativeComponent<T>();
				component.SetOwner(entity);
				component.SetWorld(this);
				return component;
			}
		}

		EntityComponent* AddComponentByTypeHash(GameEntityId id, ClassId typeHash);

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

		EntityComponent* GetComponentById(ClassId componentId, GameEntityId entityId)
		{
			if (!m_StaticState->m_ComponentInfos.Contains(componentId))
			{
				return nullptr;
			}

			auto accessorId = m_StaticState->m_ComponentInfos[componentId].AccessorId;
			return m_StaticState->m_NativeAccessors[accessorId].GetComponent(this, entityId);
		}

		static void ClearStaticState();

	private:
		void FlushPendingDestroys();

		entt::registry m_EntityRegistry;

		Array<entt::entity> m_PendingDestroyEntities;
		struct StaticState
		{
			Array<NativeComponentAccessors> m_NativeAccessors;
			Map<ClassId, NativeComponentInfo> m_ComponentInfos;
		};
		static StaticState* m_StaticState;

		Map<UUID, GameEntityId> m_UUIDToEntityMap;
		GameEntityId m_MainCamera{};
	};
}