#pragma once
#include "Core/Core.h"
#include "Rigidbody/Rigidbody.h"
#include "Renderer/World/WorldTransform.h"
#include "foundation/PxTransform.h"

namespace ke {  }

namespace physx 
{
	class PxCooking; class PxOmniPvd;
	class PxPhysics;
	class PxFoundation;
	class PxErrorCallback;
	class PxAllocatorCallback;
	class PxCpuDispatcher;
}

namespace ke
{
	class PhysicsWorld;
	class BoxShape;

	class PhysicsEngine
	{
		static PhysicsEngine* Instance;
	public:
		static PhysicsEngine* Get() { return Instance; }

		PhysicsEngine();
		~PhysicsEngine();

		inline physx::PxAllocatorCallback* GetPxAllocator() { return m_Allocator; }
		inline physx::PxErrorCallback* GetPxErrorCallback() { return m_ErrorCallback; }
		RefPtr<PhysicsWorld> CreateWorld();

		RefPtr<RigidBody> CreateRigidBody(ERigidBodyDynamics dynamicsMode, const WorldTransform& transform);
	
		static physx::PxTransform KETransformToPxTransform(const WorldTransform& transform);
		static WorldTransform PxTransformToKETransform(const physx::PxTransform& transform);
	
		void AddBoxShape(RefPtr<RigidBody> pRigidBody, float3 extent);

	private:
		physx::PxAllocatorCallback* m_Allocator{};
		physx::PxErrorCallback* m_ErrorCallback{};

		// Engine stuff
		physx::PxFoundation* m_Foundation{};
		physx::PxPhysics* m_Physics{};
		physx::PxOmniPvd* m_PvdHost{};
		physx::PxCooking* m_Cooking{};

		physx::PxCpuDispatcher* m_PhysicsDispatcher{};
		static constexpr u32 m_CpuDispatcherThreadCount = 2;

	private:
#ifdef ENABLE_DEBUG
		bool m_bRecordMemoryAllocations = true;
#else
		bool m_bRecordMemoryAllocations = false;
#endif
	};
}