#include "World.h"

namespace ke
{
	
	TWorld::TWorld(const String& InName)
		:	Name(InName)
	{
	}

	TWorld::~TWorld()
	{

	}

	void TWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
	}

}