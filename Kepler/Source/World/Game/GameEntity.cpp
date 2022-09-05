#include "GameEntity.h"
#include "GameWorld.h"
#include "Components/TransformComponent.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogEntity);

	TGameEntity::TGameEntity(TGameWorld* InWorld, entt::entity Id)
		: World(InWorld)
		, Entity(Id)
	{
		KEPLER_TRACE(LogEntity, "Created entity '{}' with Id: 0x{:x}", GetName(), GetGUID().Value);
	}

	TString TGameEntity::GetName() const
	{
		return World->GetEntityName(Entity);
	}

	id64 TGameEntity::GetGUID() const
	{
		return World->GetEntityUUID(Entity);
	}

	void TGameEntity::SetNeedsUpdate(bool bInShouldUpdate)
	{
		bShouldUpdate = bInShouldUpdate;
	}

	void TGameEntity::SetLifeSpan(float InLifeSpan)
	{
		SetNeedsUpdate(true);
		LifeSpan = InLifeSpan;
	}

	void TGameEntity::Update(float DeltaTime)
	{
		if (bPendingDestroy)
		{
			return;
		}

		LifeTime += DeltaTime;
		if (LifeSpan > -1.0f)
		{
			if (LifeTime >= LifeSpan)
			{
				KEPLER_TRACE(LogEntity, "Sent destroy command");
				World->DestroyEntity(Entity);
			}
		}
	}

	float3 TGameEntity::GetLocation() const
	{
		return World->GetComponent<TTransformComponent>(Entity).GetLocation();
	}

	float3 TGameEntity::GetRotation() const
	{
		return World->GetComponent<TTransformComponent>(Entity).GetRotation();
	}

	float3 TGameEntity::GetScale() const
	{
		return World->GetComponent<TTransformComponent>(Entity).GetScale();
	}

	void TGameEntity::SetLocation(float3 NewLocation)
	{
		World->GetComponent<TTransformComponent>(Entity).SetLocation(NewLocation);
	}

	void TGameEntity::SetRotation(float3 NewRotation)
	{
		World->GetComponent<TTransformComponent>(Entity).SetRotation(NewRotation);
	}

	void TGameEntity::SetScale(float3 NewScale)
	{
		World->GetComponent<TTransformComponent>(Entity).SetScale(NewScale);
	}

	TWorldTransform TGameEntity::GetTransform() const
	{
		return World->GetComponent<TTransformComponent>(Entity).GetTransform();
	}

	void TGameEntity::SetTransform(TWorldTransform NewTransform)
	{
		World->GetComponent<TTransformComponent>(Entity).SetTransform(NewTransform);
	}

}