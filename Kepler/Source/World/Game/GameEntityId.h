#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>

namespace ke
{
	class GameEntityId
	{
		friend class GameWorld;
		friend class TGameEntity;

	public:
		GameEntityId() : Entity(entt::null) {}
		constexpr GameEntityId(entt::entity InEntity) : Entity(InEntity) {}
		inline operator entt::entity() const { return Entity; }
		entt::entity Entity;
	};
}