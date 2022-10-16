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

	GameWorld::GameWorld(const TString& InName)
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

	TGameEntityId GameWorld::CreateEntity(const TString& Name)
	{
		const entt::entity EntityId = EntityRegistry.create();
		TNameComponent& NameComp = EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		EntityRegistry.emplace<TIdComponent>(EntityId);
		EntityRegistry.emplace<TTransformComponent>(EntityId);
		EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);
		return EntityId;
	}

	TGameEntityId GameWorld::CreateCamera(const TString& Name, float Fov, float Width, float Height, float Near, float Far)
	{
		const entt::entity EntityId = EntityRegistry.create();
		TNameComponent& NameComp = EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		EntityRegistry.emplace<TIdComponent>(EntityId);
		EntityRegistry.emplace<TTransformComponent>(EntityId);
		EntityRegistry.emplace<CameraComponent>(EntityId, Fov, Width, Height, Near, Far);
		EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);

		if (!IsValidEntity(MainCamera))
		{
			SetMainCamera(EntityId);
		}

		return EntityId;
	}

	TGameEntity& GameWorld::GetEntityFromId(TGameEntityId Id)
	{
		return EntityRegistry.get<TGameEntity>(Id.Entity);
	}

	void GameWorld::DestroyEntity(TGameEntityId Entity)
	{
		PendingDestroyEntities.EmplaceBack(Entity.Entity);
	}

	TString GameWorld::GetEntityName(TGameEntityId Entity)
	{
		if (EntityRegistry.valid(Entity.Entity))
		{
			return EntityRegistry.get<TNameComponent>(Entity.Entity).Name;
		}
		return "";
	}

	id64 GameWorld::GetEntityUUID(TGameEntityId Entity) const
	{
		if (EntityRegistry.valid(Entity.Entity))
		{
			return EntityRegistry.get<TIdComponent>(Entity.Entity).Id;
		}
		return {};
	}

	bool GameWorld::IsValidEntity(TGameEntityId Id) const
	{
		return EntityRegistry.valid(Id);
	}

	void GameWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
		KEPLER_PROFILE_SCOPE();
		if (UpdateKind != EWorldUpdateKind::Game)
		{
			return;
		}

		if (IsValidEntity(MainCamera))
		{
			auto& CameraEntity = GetEntityFromId(MainCamera);
			auto& camera = GetComponent<CameraComponent>(MainCamera).GetCamera();
			camera.SetTransform(CameraEntity.GetTransform());
		}

		// Update components for entities
		EntityRegistry.view<TMaterialComponent, TTransformComponent>().each(
			[this](auto Id, TMaterialComponent& MC, TTransformComponent& TC) 
			{
				RefPtr<TMaterial> Material = MC.GetMaterial();
				Material->WriteTransform(TC.GetTransform());
				// Material->WriteCamera(GetComponent<CameraComponent>(MainCamera).GetCamera());
				Material->WriteId((i32)Id);
			});

		EntityRegistry.view<TGameEntity>().each(
			[DeltaTime](TGameEntity& Entity)
			{
				Entity.Update(DeltaTime);
			}
		);

		FlushPendingDestroys();
	}

	void GameWorld::SetMainCamera(TGameEntityId Camera)
	{
		if (IsValidEntity(Camera))
		{
			MainCamera = Camera;
			return;
		}
		KEPLER_WARNING(LogGameWorld, "GameWorld::SetMainCamera - passed null as camera.");
	}

	bool GameWorld::IsCamera(TGameEntityId Entity) const
	{
		return HasComponent<CameraComponent>(Entity);
	}

	bool GameWorld::IsLight(TGameEntityId Entity) const
	{
		return HasComponent<AmbientLightComponent>(Entity) || HasComponent<DirectionalLightComponent>(Entity);
	}

	void GameWorld::FlushPendingDestroys()
	{
		KEPLER_PROFILE_SCOPE();
		// Flush pending destroys
		for (auto Id : PendingDestroyEntities)
		{
			EntityRegistry.destroy(Id);
		}
		PendingDestroyEntities.Clear();
	}

}