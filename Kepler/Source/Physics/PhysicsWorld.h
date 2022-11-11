#pragma once
#include "Core/Core.h"
#include "PhysicsWorld.gen.h"

namespace physx
{
	class PxScene;
}

namespace ke
{
	reflected
	class PhysicsWorld : public IntrusiveRefCounted
	{
		reflected_body();

	public:
		PhysicsWorld();
		PhysicsWorld(physx::PxScene* pScene);
		~PhysicsWorld();
		
		friend class PhysicsEngine;

		void Simulate(float deltaTime);

	private:
		bool AccumulateFixedTimeStep(float inDelta);

	private:
        physx::PxScene* m_Scene{};
		float m_AccumulatedTimeStep = 0.0f;
		float m_FixedDelta = 0.016f;
	};
}