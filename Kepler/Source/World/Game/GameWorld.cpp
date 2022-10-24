#include "GameWorld.h"
#include "GameEntity.h"
#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"
#include "../Camera/CameraComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "glm/gtc/type_ptr.inl"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogGameWorld, Info);

	GameWorld::GameWorld(const String& InName)
		: TWorld(InName)
	{
		KEPLER_INFO(LogGameWorld, "Created GameWorld with name {}", InName);
		bNeedsUpdate = true;
	}

	GameWorld::~GameWorld()
	{
		FlushPendingDestroys();
		KEPLER_INFO(LogGameWorld, "Destroyed GameWorld with name {}", Name);
	}

	GameEntityId GameWorld::CreateEntity(const String& Name)
	{
		const GameEntityId EntityId = m_EntityRegistry.create();
		TNameComponent& NameComp = m_EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		AddComponent<TIdComponent>(EntityId);
		AddComponent<TransformComponent>(EntityId);
		m_EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);
		return EntityId;
	}

	GameEntityId GameWorld::CreateCamera(const String& Name, float Fov, float Width, float Height, float Near, float Far)
	{
		const entt::entity EntityId = m_EntityRegistry.create();
		TNameComponent& NameComp = m_EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		AddComponent<TIdComponent>(EntityId);
		AddComponent<TransformComponent>(EntityId);
		AddComponent<CameraComponent>(EntityId, Fov, Width, Height, Near, Far);
		m_EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);

		if (!IsValidEntity(m_MainCamera))
		{
			SetMainCamera(EntityId);
		}

		return EntityId;
	}

	TGameEntity& GameWorld::GetEntityFromId(GameEntityId Id)
	{
		return m_EntityRegistry.get<TGameEntity>(Id.Entity);
	}

	void GameWorld::DestroyEntity(GameEntityId Entity)
	{
		m_PendingDestroyEntities.EmplaceBack(Entity.Entity);
	}

	String GameWorld::GetEntityName(GameEntityId Entity)
	{
		if (m_EntityRegistry.valid(Entity.Entity))
		{
			return m_EntityRegistry.get<TNameComponent>(Entity.Entity).Name;
		}
		return "";
	}

	id64 GameWorld::GetEntityUUID(GameEntityId Entity) const
	{
		if (m_EntityRegistry.valid(Entity.Entity))
		{
			return m_EntityRegistry.get<TIdComponent>(Entity.Entity).Id;
		}
		return {};
	}

	bool GameWorld::IsValidEntity(GameEntityId Id) const
	{
		return m_EntityRegistry.valid(Id);
	}

	void GameWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
		KEPLER_PROFILE_SCOPE();
		if (IsValidEntity(m_MainCamera))
		{
			auto& CameraEntity = GetEntityFromId(m_MainCamera);
			auto& camera = GetComponent<CameraComponent>(m_MainCamera).GetCamera();
			camera.SetTransform(CameraEntity.GetTransform());
		}

		// Update components for entities
		m_EntityRegistry.view<MaterialComponent, TransformComponent>().each(
			[this](auto Id, MaterialComponent& MC, TransformComponent& TC) 
			{
				RefPtr<TMaterial> Material = MC.GetMaterial();
				Material->WriteTransform(TC.GetTransform());
				// Material->WriteCamera(GetComponent<CameraComponent>(MainCamera).GetCamera());
				Material->WriteId((i32)Id);
			});

		m_EntityRegistry.view<TGameEntity>().each(
			[DeltaTime](TGameEntity& Entity)
			{
				Entity.Update(DeltaTime);
			}
		);

		if (UpdateKind == EWorldUpdateKind::Play)
		{
			for (auto& accessor : m_NativeAccessors)
			{
				accessor.OnUpdate(DeltaTime);
			}
		}

		FlushPendingDestroys();
	}

	void GameWorld::SetMainCamera(GameEntityId Camera)
	{
		if (IsValidEntity(Camera))
		{
			m_MainCamera = Camera;
			return;
		}
		KEPLER_WARNING(LogGameWorld, "GameWorld::SetMainCamera - passed null as camera.");
	}

	bool GameWorld::IsCamera(GameEntityId Entity) const
	{
		return HasComponent<CameraComponent>(Entity);
	}

	bool GameWorld::IsLight(GameEntityId Entity) const
	{
		return HasComponent<AmbientLightComponent>(Entity) || HasComponent<DirectionalLightComponent>(Entity);
	}

	void GameWorld::FlushPendingDestroys()
	{
		KEPLER_PROFILE_SCOPE();
		// Flush pending destroys
		for (auto Id : m_PendingDestroyEntities)
		{
			m_EntityRegistry.destroy(Id);
		}
		m_PendingDestroyEntities.Clear();
	}

}