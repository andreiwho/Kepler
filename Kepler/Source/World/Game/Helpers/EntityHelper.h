#pragma once
#include "Core/Core.h"
#include "World/Game/GameEntity.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	// NEVER SAVE THIS VALUE
	// For in-place use only
	class EntityHandle
	{
	public:
		EntityHandle(GameWorld* world, GameEntityId id);

		template<typename T>
		T* GetComponent() 
		{ 
			if (!m_GameWorld || !m_Entity)
			{
				return nullptr;
			}

			if (!HasComponent<T>())
			{
				return nullptr;
			}

			return &m_GameWorld->GetComponent<T>(m_Id);
		}

		template<typename T>
		const T* GetComponent() const
		{
			if (!m_GameWorld || !m_Entity)
			{
				return nullptr;
			}

			if (!HasComponent<T>())
			{
				return nullptr;
			}

			return &m_GameWorld->GetComponent<T>();
		}

		template<typename T, typename ... ARGS>
		T* AddComponent(ARGS&&... Args)
		{
			if (!m_GameWorld || !m_Entity)
			{
				return nullptr;
			}

			return &m_GameWorld->AddComponent<T>(m_Id, std::forward<ARGS>(Args)...);
		}

		template<typename T>
		inline bool HasComponent() const
		{
			return m_GameWorld->HasComponent<T>(m_Id);
		}

		inline operator bool() const { return !!m_Entity && !!m_GameWorld; }
		inline operator GameEntityId() const { return m_Id; }

		TGameEntity* operator->() { return m_Entity; }
		const TGameEntity* operator->() const { return m_Entity; }
		TGameEntity& operator*() { CHECK(*this); return *m_Entity; }
		const TGameEntity& operator*() const { CHECK(*this); return *m_Entity; }

	private:
		GameEntityId m_Id;
		GameWorld* m_GameWorld;
		TGameEntity* m_Entity{};
	};
}