#pragma once
#include "Core/Core.h"
#include "World.gen.h"

namespace ke
{
	reflected enum class EWorldUpdateKind
	{
		Play,
		Edit
	};

	// World - base class for all Entity-combined objects, such as:
	// - GameWorlds,
	// - PhysicsWorlds
	// - Prefabs
	// - etc (there can be a variety of world-like objects...)
	// The purpose of the world is to manage Entities and their components
	// The main reason of this being a distinct parent class is that worlds can 
	// each-other handle different kinds of entities, thus, having different kinds of registries and components
	// This is a subject to change
	reflected class TWorld : public EnableRefPtrFromThis<TWorld>
	{
		reflection_info();
	public:
		TWorld() = default;

		~TWorld();

		const String& GetName() const { return Name; }

		reflected String Name{"NoName"};
	protected:
		// The constructor
		TWorld(const String& Name);
		
		// Every world should override this function in order to produce some functionality
		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind);

		// The name of the world

		// The UUID of the world (will be used mostly for parent-child behaviour)
		UUID m_UUID{};

		// Does this world need an delta update
		bool bNeedsUpdate = false;
	};
}