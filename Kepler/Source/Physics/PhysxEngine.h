#pragma once
#include "Core/Core.h"

namespace physx 
{
	class PxCooking; class PxOmniPvd;
	class PxPhysics;
	class PxFoundation;
	class PxErrorCallback;
	class PxAllocatorCallback;
}

namespace ke
{
	class PhysicsWorld;

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
	private:
		physx::PxAllocatorCallback* m_Allocator{};
		physx::PxErrorCallback* m_ErrorCallback{};

		// Engine stuff
		physx::PxFoundation* m_Foundation{};
		physx::PxPhysics* m_Physics{};
		physx::PxOmniPvd* m_PvdHost{};
		physx::PxCooking* m_Cooking{};

	private:
#ifdef ENABLE_DEBUG
		bool m_bRecordMemoryAllocations = true;
#else
		bool m_bRecordMemoryAllocations = false;
#endif
	};
}