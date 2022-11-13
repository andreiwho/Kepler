#include "RigidbodyComponent.h"
#include "Physics/PhysxEngine.h"
#include "../../Helpers/EntityHelper.h"
#include "../StaticMeshComponent.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogRigidbodyComponent, All);

	void RigidbodyComponent::OnAttach()
	{
		Base::OnAttach();

		if (GameWorld* pWorld = GetWorld())
		{
			EntityHandle handle{ GetWorld(), GetOwner() };
			m_RigidBody = PhysicsEngine::Get()->CreateRigidBody(ERigidBodyDynamics::Static, handle->GetTransform());
			ConfigureRigidBody();

			if (RefPtr<PhysicsWorld> pPhysicsWorld = pWorld->GetPhysicsWorld())
			{
				pPhysicsWorld->AddRigidBody(m_RigidBody);
			}
		}
	}

	void RigidbodyComponent::SetDynamicsMode(ERigidBodyDynamics newMode)
	{
		if (m_DynamicsMode == newMode)
		{
			return;
		}

		KEPLER_TRACE(LogRigidbodyComponent, "Changed rigidbody dynamics mode to {}", GetReflectedEnum<ERigidBodyDynamics>()->ValueToString((i32)newMode));
		if (m_RigidBody)
		{
			if (GameWorld* pWorld = GetWorld())
			{
				if (auto pPxWorld = pWorld->GetPhysicsWorld())
				{
					EntityHandle handle{ GetWorld(), GetOwner() };

					pPxWorld->RemoveRigidBody(m_RigidBody);
					m_RigidBody = PhysicsEngine::Get()->CreateRigidBody(newMode, handle->GetTransform());
					ConfigureRigidBody();
					pPxWorld->AddRigidBody(m_RigidBody);
				}
			}
		}

		m_DynamicsMode = newMode;
	}

	void RigidbodyComponent::SetMass(float newMass)
	{
		if (m_RigidBody)
		{
			m_RigidBody->SetMass(newMass);
		}

		m_Mass = newMass;
	}

	void RigidbodyComponent::SetEnableGravity(bool bNewEnableGravity)
	{
		if (m_RigidBody)
		{
			m_RigidBody->SetEnableGravity(bNewEnableGravity);
		}

		m_EnableGravity = bNewEnableGravity;
	}

	void RigidbodyComponent::SetAngularDamping(float newDamping)
	{
		if (m_RigidBody)
		{
			m_RigidBody->SetAngularDamping(newDamping);
		}
		m_AngularDamping = newDamping;
	}

	void RigidbodyComponent::PostSimulate()
	{
		if (m_RigidBody)
		{
			const WorldTransform newTransform = m_RigidBody->GetSimulatedTransform();
			EntityHandle handle{ GetWorld(), GetOwner() };
			const WorldTransform newEntityTransform = WorldTransform(newTransform.GetLocation(), newTransform.GetRotation(), handle->GetScale());
			handle->SetTransform(newEntityTransform);
		}
	}

	void RigidbodyComponent::ConfigureRigidBody()
	{
		if (m_RigidBody)
		{
			m_RigidBody->SetMass(m_Mass);
			m_RigidBody->SetEnableGravity(m_EnableGravity);
			m_RigidBody->SetAngularDamping(m_AngularDamping);


			float3 bounds = { 0.5f, 0.5f, 0.5f };
			if (GetWorld()->HasComponent<StaticMeshComponent>(GetOwner()))
			{
				EntityHandle handle{ GetWorld(), GetOwner() };

				auto& pSMC = GetWorld()->GetComponent<StaticMeshComponent>(GetOwner());
				if (auto pStaticMesh = pSMC.GetStaticMesh())
				{
					bounds = pStaticMesh->GetBoundingBoxSize();
					bounds *= handle->GetScale();
				}
			}

			PhysicsEngine::Get()->AddBoxShape(m_RigidBody, bounds);
		}
	}

}