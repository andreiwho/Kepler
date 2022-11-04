#pragma once
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	class EntityDetailsPanel
	{
	public:
		EntityDetailsPanel(RefPtr<GameWorld> pWorld, GameEntityId entity);
		
		void Draw();

	private:
		void DrawTransformComponentInfo();
		void DrawMaterialComponentInfo();
		void DrawEntityInfo();
		void DrawNativeComponentInfo();
		void DrawAddComponentPopup();
		const String& SplitAndCapitalizeComponentName(const String& originalName);

	private:
		RefPtr<GameWorld> m_pWorld;
		GameEntityId m_SelectedEntity;
		Map<String, String> m_FilteredComponentNames;
	};
}