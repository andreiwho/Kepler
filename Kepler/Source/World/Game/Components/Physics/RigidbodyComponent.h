#pragma once
#include "World/Game/Components/EntityComponent.h"
#include "Physics/Rigidbody/Rigidbody.h"
#include "RigidbodyComponent.gen.h"

namespace ke
{
	reflected class RigidbodyComponent : public EntityComponent
	{
		reflected_body();

	public:
		RigidbodyComponent();
		
	private:
		RefPtr<RigidBody> m_RigidBody;

		reflected kmeta(set = SetDynamicsMode)
		ERigidBodyDynamics m_DynamicsMode {};
		void SetDynamicsMode(ERigidBodyDynamics newMode);

	};
}