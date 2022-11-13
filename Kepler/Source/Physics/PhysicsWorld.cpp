#include "PhysicsWorld.h"
#include "PxScene.h"
#include "PxRigidActor.h"
#include "PhysxEngine.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogPxWorld, All);

	PhysicsWorld::PhysicsWorld(physx::PxScene* pScene)
		: m_Scene(pScene)
	{
	}

	PhysicsWorld::~PhysicsWorld()
	{
		if (m_Scene)
		{
			m_Scene->release();
		}
	}

	void PhysicsWorld::AddRigidBody(RefPtr<RigidBody> pRigidBody)
	{
		m_Scene->addActor(*pRigidBody->m_RigidBody);
		KEPLER_TRACE(LogPxWorld, "Added rigidbody to physics world");
	}

	void PhysicsWorld::RemoveRigidBody(RefPtr<RigidBody> pRigidBody)
	{
		if (pRigidBody && pRigidBody->m_RigidBody)
		{
			m_Scene->removeActor(*pRigidBody->m_RigidBody);
			KEPLER_TRACE(LogPxWorld, "Removed rigidbody from physics world");
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