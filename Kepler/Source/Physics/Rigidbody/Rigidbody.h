#pragma once
#include "Core/Core.h"
#include "Rigidbody.gen.h"

namespace physx
{
	class PxScene;
	class PxRigidActor;
}

namespace ke
{
	reflected 
	enum class ERigidBodyDynamics
	{
		Static,
		Dynamic,
	};

	class RigidBody : public IntrusiveRefCounted
	{
	public:
		RigidBody(ERigidBodyDynamics initialDynamics, physx::PxRigidActor* pActor);
		~RigidBody();

		inline ERigidBodyDynamics GetDynamicsMode() const { return m_DynamicsMode; }

	private:
		physx::PxRigidActor* m_RigidBody{};
		ERigidBodyDynamics m_DynamicsMode{ERigidBodyDynamics::Static};
	};
}