#include "Rigidbody.h"
#include "PxRigidActor.h"

namespace ke
{
	RigidBody::RigidBody(ERigidBodyDynamics initialDynamics, physx::PxRigidActor* pActor)
		: m_RigidBody(pActor)
		, m_DynamicsMode(initialDynamics)
	{
	}

	RigidBody::~RigidBody()
	{
		if (m_RigidBody)
		{
			m_RigidBody->release();
		}
	}
}