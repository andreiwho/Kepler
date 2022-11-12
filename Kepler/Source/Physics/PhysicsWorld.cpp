#include "PhysicsWorld.h"
#include "PxScene.h"
#include "PhysxEngine.h"

namespace ke
{
	PhysicsWorld::PhysicsWorld(physx::PxScene* pScene)
		:	m_Scene(pScene)
	{
	}

	PhysicsWorld::~PhysicsWorld()
	{
		if (m_Scene)
		{
			m_Scene->release();
		}
	}

	void PhysicsWorld::Simulate(float deltaTime)
	{
		if (AccumulateFixedTimeStep(deltaTime))
		{
			m_Scene->simulate(m_FixedDelta);
			m_Scene->fetchResults(true);
		}
	}

	bool PhysicsWorld::AccumulateFixedTimeStep(float inDelta)
	{
		m_AccumulatedTimeStep += inDelta;
		if (m_AccumulatedTimeStep >= m_FixedDelta)
		{
			m_AccumulatedTimeStep = 0.0f;
			return true;
		}
		return false;
	}

}