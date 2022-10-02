#include "GameWorld.h"
#include "GameEntity.h"
#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"
#include "../Camera/CameraComponent.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogGameWorld);

	TGameWorld::TGameWorld(const TString& InName)
		: TWorld(InName)
	{
		KEPLER_INFO(LogGameWorld, "Created GameWorld with name {}", InName);
		bNeedsUpdate = true;
	}

	TGameWorld::~TGameWorld()
	{
		FlushPendingDestroys();
		KEPLER_INFO(LogGameWorld, "Destroyed GameWorld with name {}", Name);
	}

	TGameEntityId TGameWorld::CreateEntity(const TString& Name)
	{
		const entt::entity EntityId = EntityRegistry.create();
		TNameComponent& NameComp = EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		EntityRegistry.emplace<TIdComponent>(EntityId);
		EntityRegistry.emplace<TTransformComponent>(EntityId);
		EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);
		return EntityId;
	}

	TGameEntityId TGameWorld::CreateCamera(const TString& Name, float Fov, float Width, float Height, float Near, float Far)
	{
		const entt::entity EntityId = EntityRegistry.create();
		TNameComponent& NameComp = EntityRegistry.emplace<TNameComponent>(EntityId);
		NameComp.Name = Name;
		EntityRegistry.emplace<TIdComponent>(EntityId);
		EntityRegistry.emplace<TTransformComponent>(EntityId);
		EntityRegistry.emplace<TCameraComponent>(EntityId, Fov, Width, Height, Near, Far);
		EntityRegistry.emplace<TGameEntity>(EntityId, this, EntityId);

		if (!IsValidEntity(MainCamera))
		{
			SetMainCamera(EntityId);
		}

		return EntityId;
	}

	TGameEntity& TGameWorld::GetEntityFromId(TGameEntityId Id)
	{
		return EntityRegistry.get<TGameEntity>(Id.Entity);
	}

	void TGameWorld::DestroyEntity(TGameEntityId Entity)
	{
		PendingDestroyEntities.EmplaceBack(Entity.Entity);
	}

	TString TGameWorld::GetEntityName(TGameEntityId Entity)
	{
		if (EntityRegistry.valid(Entity.Entity))
		{
			return EntityRegistry.get<TNameComponent>(Entity.Entity).Name;
		}
		return "";
	}

	id64 TGameWorld::GetEntityUUID(TGameEntityId Entity) const
	{
		if (EntityRegistry.valid(Entity.Entity))
		{
			return EntityRegistry.get<TIdComponent>(Entity.Entity).Id;
		}
		return {};
	}

	bool TGameWorld::IsValidEntity(TGameEntityId Id) const
	{
		return EntityRegistry.valid(Id);
	}

	void TGameWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
		KEPLER_PROFILE_SCOPE();
		if (UpdateKind != EWorldUpdateKind::Game)
		{
			return;
		}

		if (IsValidEntity(MainCamera))
		{
			auto& CameraEntity = GetEntityFromId(MainCamera);
			auto& Camera = GetComponent<TCameraComponent>(MainCamera).GetCamera();
			Camera.SetTransform(CameraEntity.GetTransform());
		}

		// Update components for entities
		EntityRegistry.view<TMaterialComponent, TTransformComponent>().each(
			[this](auto Id, TMaterialComponent& MC, TTransformComponent& TC) 
			{
				TRef<TMaterial> Material = MC.GetMaterial();
				Material->WriteTransform(TC.GetTransform());
				Material->WriteCamera(GetComponent<TCameraComponent>(MainCamera).GetCamera());
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

	void TGameWorld::SetMainCamera(TGameEntityId Camera)
	{
		if (IsValidEntity(Camera))
		{
			MainCamera = Camera;
			return;
		}
		KEPLER_WARNING(LogGameWorld, "GameWorld::SetMainCamera - passed null as camera.");
	}

	bool TGameWorld::IsCamera(TGameEntityId Entity) const
	{
		return HasComponent<TCameraComponent>(Entity);
	}

	void TGameWorld::FlushPendingDestroys()
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