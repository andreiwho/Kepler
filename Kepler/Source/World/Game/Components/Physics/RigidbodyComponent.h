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
		virtual void OnAttach() override;

		void SetDynamicsMode(ERigidBodyDynamics newMode);
		ERigidBodyDynamics GetDynamicsMode() const;
		
		void SetMass(float newMass);
		void SetEnableGravity(bool bNewEnableGravity);
		void SetAngularDamping(float newDamping);


		void PostSimulate();

	private:
		void ConfigureRigidBody();

	private:
		RefPtr<RigidBody> m_RigidBody;

		reflected kmeta(set = SetDynamicsMode, get = GetDynamicsMode)
		ERigidBodyDynamics m_DynamicsMode {};

		reflected kmeta(set = SetMass) 
		float m_Mass{1.0f};

		reflected kmeta(set = SetAngularDamping)
		float m_AngularDamping{ 1.0f };

		reflected kmeta(set = SetEnableGravity)
		bool m_EnableGravity{ true };
	};
}