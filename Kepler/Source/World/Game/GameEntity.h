#pragma once
#include "Core/Core.h"
#include "GameEntityId.h"
#include "Renderer/World/WorldTransform.h"

namespace ke
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

		void SetName(const TString& Name);

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

		inline bool IsCamera() const { return bIsCamera; }

		inline void SetHideInSceneGraph(bool bHide)
		{
			bHideInSceneGraph = bHide;
		}

		inline bool ShouldHideInSceneGraph() const { return bHideInSceneGraph; }

		float3 GetForwardVector() const;
		
		float3 GetRightVector() const;
		
		float3 GetUpVector() const;

	private:
		bool bHideInSceneGraph = false;

		friend class TGameWorld;
		TGameEntityId Internal_GetId() const { return Entity; }
		
		TGameWorld* World{};
		TGameEntityId Entity{};
		
		float LifeSpan = -1.0f;
		float LifeTime = 0.0f;

		u8 bShouldUpdate : 1 = false;
		u8 bPendingDestroy : 1 = false;
		u8 bIsCamera : 1 = false;
	};
}