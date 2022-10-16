#pragma once
#include "Core/Core.h"

namespace ke
{
	enum class EWorldUpdateKind
	{
		Game,
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
	class TWorld : public EnableRefPtrFromThis<TWorld>
	{
	public:
		~TWorld();

		void Internal_Update(float DeltaTime);

		const TString& GetName() const { return Name; }

	protected:
		// The constructor
		TWorld(const TString& Name);
		
		// Every world should override this function in order to produce some functionality
		virtual void UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind);

		// The name of the world
		TString Name{"NoName"};

		// The UUID of the world (will be used mostly for parent-child behaviour)
		id64 UUID{};

		// Does this world need an delta update
		bool bNeedsUpdate = false;
	};
}