#pragma once
#include "Core/Core.h"
#include "Rigidbody.gen.h"
#include "Renderer/World/WorldTransform.h"

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
		friend class PhysicsEngine;
		friend class PhysicsWorld;
	public:
		RigidBody(ERigidBodyDynamics initialDynamics, physx::PxRigidActor* pActor);
		~RigidBody();

		inline ERigidBodyDynamics GetDynamicsMode() const { return m_DynamicsMode; }
		WorldTransform GetSimulatedTransform() const;

		void SetMass(float newMass);
		void SetEnableGravity(bool bNewEnableGravity);
		void SetAngularDamping(float newDamping);

	private:
		physx::PxRigidActor* m_RigidBody{};
		ERigidBodyDynamics m_DynamicsMode{ERigidBodyDynamics::Static};
	};
}