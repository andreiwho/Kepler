#pragma once
#include "Core/Core.h"
#include "GameEntityId.h"
#include "Renderer/World/WorldTransform.h"

namespace Kepler
{
	class TGameWorld;

	struct TIdComponent
	{
		id64 Id;
	};

	struct TNameComponent
	{
		TString Name;
	};

	class TGameEntity
	{
	public:
		TGameEntity(TGameWorld* InWorld, entt::entity Id);

		TString GetName() const;

		id64 GetGUID() const;

		inline TRef<TGameWorld> GetWorld() const { return World; }

		void SetNeedsUpdate(bool bInShouldUpdate);
		
		void SetLifeSpan(float LifeSpan);

		void Update(float DeltaTime);

		float3 GetLocation() const;
		
		float3 GetRotation() const;

		float3 GetScale() const;

		void SetLocation(float3 NewLocation);
		
		void SetRotation(float3 NewRotation);

		void SetScale(float3 NewScale);

		TWorldTransform GetTransform() const;

		void SetTransform(TWorldTransform NewTransform);

	private:
		friend class TGameWorld;
		TGameEntityId Internal_GetId() const { return Entity; }
		
		TGameWorld* World{};
		TGameEntityId Entity{};
		
		float LifeSpan = -1.0f;
		float LifeTime = 0.0f;

		bool bShouldUpdate = false;
		bool bPendingDestroy = false;
	};
}