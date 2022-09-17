#pragma once
#include "Core/Core.h"
#include "World/Game/GameEntity.h"
#include "World/Game/GameWorld.h"

namespace Kepler
{
	// NEVER SAVE THIS VALUE
	// For in-place use only
	class TEntityHandle
	{
	public:
		TEntityHandle(TRef<TGameWorld> World, TGameEntityId EntityId);
		
		template<typename T>
		T* GetComponent() 
		{ 
			if (!GameWorld || !Entity)
			{
				return nullptr;
			}

			if (!HasComponent<T>())
			{
				return nullptr;
			}

			return &GameWorld->GetComponent<T>(Id);
		}

		template<typename T>
		const T* GetComponent() const
		{
			if (!GameWorld || !Entity)
			{
				return nullptr;
			}

			if (!HasComponent<T>())
			{
				return nullptr;
			}

			return &GameWorld->GetComponent<T>();
		}

		template<typename T, typename ... ARGS>
		T* AddComponent(ARGS&&... Args)
		{
			if (!GameWorld || !Entity)
			{
				return nullptr;
			}

			return &GameWorld->AddComponent<T>(Id, std::forward<ARGS>(Args)...);
		}

		template<typename T>
		inline bool HasComponent() const
		{
			return GameWorld->HasComponent<T>(Id);
		}

		inline operator bool() const { return !!Entity && !!GameWorld; }
		inline operator TGameEntityId() const { return Id; }

		TGameEntity* operator->() { return Entity; }
		const TGameEntity* operator->() const { return Entity; }
		TGameEntity& operator*() { CHECK(*this); return *Entity; }
		const TGameEntity& operator*() const { CHECK(*this); return *Entity; }

	private:
		TGameEntityId Id;
		TGameWorld* GameWorld;
		TGameEntity* Entity{};
	};
}