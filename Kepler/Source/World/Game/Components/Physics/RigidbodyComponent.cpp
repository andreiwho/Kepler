#include "RigidbodyComponent.h"

namespace ke
{
	RigidbodyComponent::RigidbodyComponent()
	{
		//m_RigidBody = MakeRef(New<RigidBody>(ERigidBodyDynamics::Static));
		CHECK(m_RigidBody);

		m_DynamicsMode = m_RigidBody->GetDynamicsMode();
	}

	void RigidbodyComponent::SetDynamicsMode(ERigidBodyDynamics newMode)
	{
		if (m_DynamicsMode == newMode)
		{
			return;
		}

	}

}