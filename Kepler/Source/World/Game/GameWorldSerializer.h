#pragma once
#include "GameWorld.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class GameWorldSerializer
	{
	public:
		GameWorldSerializer(RefPtr<GameWorld> pWorld);

		inline const String& GetWorldName() const& { return m_Name; }

	private:
		RefPtr<GameWorld> m_World;
		String m_Name;
	};
}