#include "GameWorld.h"
#include "GameEntity.h"
#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"

namespace Kepler
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

		TGameEntityId Id{};
		Id.Entity = EntityId;
		return Id;
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

	void TGameWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
		if (UpdateKind != EWorldUpdateKind::Game)
		{
			return;
		}

		// Update components for entities
		EntityRegistry.view<TMaterialComponent, TTransformComponent>().each(
			[this](auto, TMaterialComponent& MC, TTransformComponent& TC) 
			{
				TRef<TMaterial> Material = MC.GetMaterial();
				Material->WriteTransform(TC.GetTransform());
			});

		EntityRegistry.view<TGameEntity>().each(
			[DeltaTime](TGameEntity& Entity)
			{
				Entity.Update(DeltaTime);
			}
		);

		FlushPendingDestroys();
	}

	void TGameWorld::FlushPendingDestroys()
	{
		// Flush pending destroys
		for (auto Id : PendingDestroyEntities)
		{
			EntityRegistry.destroy(Id);
		}
		PendingDestroyEntities.Clear();
	}

}