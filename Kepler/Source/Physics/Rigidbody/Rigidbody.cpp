#include "Rigidbody.h"
#include "PxRigidActor.h"
#include "../PhysxEngine.h"
#include "PxRigidDynamic.h"
#include "PxActor.h"

namespace ke
{
	RigidBody::RigidBody(ERigidBodyDynamics initialDynamics, physx::PxRigidActor* pActor)
		: m_RigidBody(pActor)
		, m_DynamicsMode(initialDynamics)
	{
	}

	RigidBody::~RigidBody()
	{
		if (m_RigidBody && m_RigidBody->isReleasable())
		{
			m_RigidBody->release();
		}
	}

	WorldTransform RigidBody::GetSimulatedTransform() const
	{
		CHECK(m_RigidBody);
		physx::PxTransform pxTransform = m_RigidBody->getGlobalPose();
		return PhysicsEngine::PxTransformToKETransform(pxTransform);
	}

	void RigidBody::SetMass(float newMass)
	{
		if (!m_RigidBody)
		{
			return;
		}

		if (auto pDynamic = m_RigidBody->is<physx::PxRigidDynamic>())
		{
			pDynamic->setMass(newMass);
		}
	}

	void RigidBody::SetEnableGravity(bool bNewEnableGravity)
	{
		if (!m_RigidBody)
		{
			return;
		}

		if (auto pDynamic = m_RigidBody->is<physx::PxRigidDynamic>())
		{
			pDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !bNewEnableGravity);
		}
	}

	void RigidBody::SetAngularDamping(float newDamping)
	{
		if (!m_RigidBody)
		{
			return;
		}

		if (auto pDynamic = m_RigidBody->is<physx::PxRigidDynamic>())
		{
			pDynamic->setAngularDamping(newDamping);
		}
	}

}