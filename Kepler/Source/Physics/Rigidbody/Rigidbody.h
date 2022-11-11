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
	reflected class RigidBody : public Object
	{
		reflected_body();
	
	public:
		
	private:
		physx::PxScene* m_Scene{};
		physx::PxRigidActor* m_RigidBody{};
	};
}