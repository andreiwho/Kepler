#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>

namespace ke
{
	class TGameEntityId
	{
		friend class GameWorld;
		friend class TGameEntity;

	public:
		TGameEntityId() : Entity(entt::null) {}
		constexpr TGameEntityId(entt::entity InEntity) : Entity(InEntity) {}
		inline operator entt::entity() const { return Entity; }
		entt::entity Entity;
	};
}