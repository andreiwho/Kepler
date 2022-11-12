#include "PhysxEngine.h"
#include "foundation/PxErrors.h"
#include "foundation/PxErrorCallback.h"
#include "foundation/PxFoundation.h"
#include "foundation/PxPhysicsVersion.h"
#include "common/PxTolerancesScale.h"
#include "pvd/PxPvdTransport.h"
#include "omnipvd/PxOmniPvd.h"
#include "foundation/PxAllocatorCallback.h"
#include "PxPhysics.h"
#include "cooking/PxCooking.h"
#include "PxSceneDesc.h"
#include "PhysicsWorld.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "PxRigidActor.h"
#include "PxRigidStatic.h"
#include "PxRigidDynamic.h"

namespace
{
	template<typename ... Types>
	void pxOrderedRelease(Types*... objects)
	{
		(objects->release(), ...);
	}
}

namespace ke
{	
	DEFINE_UNIQUE_LOG_CHANNEL(LogPhysics, All);

	//////////////////////////////////////////////////////////////////////////
	//	ALLOCATOR
	//////////////////////////////////////////////////////////////////////////
	class KeplerPhysXAllocator : public physx::PxAllocatorCallback
	{
	public:
		// Allocation must be 16 bytes alligned, so this is a subject to consider adding the alignment option to the TMalloc
		virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override
		{
			return TMalloc::Get()->Allocate(size);
		}

		virtual void deallocate(void* ptr) override
		{
			TMalloc::Get()->Free(ptr);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//	ERROR CALLBACK
	//////////////////////////////////////////////////////////////////////////
	class KeplerPhysXErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			switch (code)
			{
			case physx::PxErrorCode::eNO_ERROR:
				KEPLER_TRACE(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eDEBUG_INFO:
				KEPLER_TRACE(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eDEBUG_WARNING:
				KEPLER_WARNING(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eINVALID_PARAMETER:
				KEPLER_ERROR_STOP(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eINVALID_OPERATION:
				KEPLER_ERROR_STOP(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eOUT_OF_MEMORY:
				KEPLER_ERROR_STOP(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eINTERNAL_ERROR:
				KEPLER_ERROR_STOP(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eABORT:
				KEPLER_ERROR_STOP(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::ePERF_WARNING:
				KEPLER_WARNING(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			case physx::PxErrorCode::eMASK_ALL:
				KEPLER_CRITICAL(LogPhysics, "PhysX: {} in file {} on line {}", message, file, line);
				break;
			default:
				break;
			}
		}
	};


	//////////////////////////////////////////////////////////////////////////
	//	ENGINE
	//////////////////////////////////////////////////////////////////////////
	PhysicsEngine* PhysicsEngine::Instance = nullptr;
	PhysicsEngine::PhysicsEngine()
	{
		Instance = this;

		// Initialize the allocator
		m_Allocator = New<KeplerPhysXAllocator>();
	
		// Initialize the error callback
		m_ErrorCallback = New<KeplerPhysXErrorCallback>();

		// initailize foundation
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_Allocator, *m_ErrorCallback);
		CHECK(m_Foundation);

		// Initialize pvd
		m_PvdHost = PxCreateOmniPvd(*m_Foundation);
		CHECK(m_PvdHost);

		// Initalize physics
		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), m_bRecordMemoryAllocations, nullptr, m_PvdHost);
		CHECK(m_Physics);

		// Initlaize cooking
		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
		CHECK(m_Cooking);

		// Initialize physics dispatcher
		m_PhysicsDispatcher = physx::PxDefaultCpuDispatcherCreate(m_CpuDispatcherThreadCount);
	}

	PhysicsEngine::~PhysicsEngine()
	{
		pxOrderedRelease(m_Cooking, m_Physics, m_PvdHost, m_Foundation);

		// Free the error callback
		if (m_ErrorCallback)
		{
			Delete(m_ErrorCallback);
		}

		// Free the allocator
		if (m_Allocator)
		{
			Delete(m_Allocator);
		}
	}

	RefPtr<PhysicsWorld> PhysicsEngine::CreateWorld()
	{
		physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
		sceneDesc.gravity = { 0.0f, -9.81f, 0.0f };
		sceneDesc.cpuDispatcher = m_PhysicsDispatcher;
		sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
		// TODO: Configure the properties needed
		physx::PxScene* pScene = m_Physics->createScene(sceneDesc);

		CHECK(pScene);
		return New<PhysicsWorld>(pScene);
	}

	RefPtr<RigidBody> PhysicsEngine::CreateRigidBody(ERigidBodyDynamics dynamicsMode, const WorldTransform& transform)
	{
		physx::PxRigidActor* pActor = nullptr;
		switch (dynamicsMode)
		{
		case ke::ERigidBodyDynamics::Static:
			pActor = m_Physics->createRigidStatic({});
			break;
		case ke::ERigidBodyDynamics::Dynamic:
			pActor = m_Physics->createRigidDynamic({});
			break;
		default:
			break;
		}
		return {};
	}

	physx::PxTransform PhysicsEngine::KETransformToPxTransform(const WorldTransform& transform)
	{
		const float3 location = transform.GetLocation();
		const physx::PxVec3 pxLocation{ location.x, location.y, location.z };
		const quaternion rotation = transform.RotationToQuat();
		const physx::PxQuat pxRotation{ rotation.x, rotation.y, rotation.z, rotation.w };
		return physx::PxTransform(pxLocation, pxRotation);
	}

	WorldTransform PhysicsEngine::PxTransformToKETransform(const physx::PxTransform& transform)
	{
		const auto& pxPosition = transform.p;
		const auto& pxQuat = transform.q;

		

		return {};
	}

}