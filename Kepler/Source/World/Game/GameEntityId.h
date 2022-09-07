#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>

namespace Kepler
{
	class TGameEntityId
	{
		friend class TGameWorld;
		friend class TGameEntity;

	public:
		TGameEntityId() : Entity(entt::null) {}

	private:
		constexpr TGameEntityId(entt::entity InEntity) : Entity(InEntity) {}
		inline operator entt::entity() const { return Entity; }

		entt::entity Entity;
	};
}