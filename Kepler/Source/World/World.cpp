#include "World.h"

namespace ke
{
	
	TWorld::TWorld(const TString& InName)
		:	Name(InName)
	{
	}

	TWorld::~TWorld()
	{

	}

	void TWorld::Internal_Update(float DeltaTime)
	{
		if (bNeedsUpdate)
		{
			UpdateWorld(DeltaTime, EWorldUpdateKind::Game);
		}
	}

	void TWorld::UpdateWorld(float DeltaTime, EWorldUpdateKind UpdateKind)
	{
	}

}