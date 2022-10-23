#pragma once
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	class TEditorDetailsPanel
	{
	public:
		TEditorDetailsPanel(RefPtr<GameWorld> pWorld, GameEntityId entity);
		
		void Draw();

	private:
		void DrawTransformComponentInfo();
		void DrawCameraComponentInfo();
		void DrawMaterialComponentInfo();
		void DrawLightInfo();
		void DrawEntityInfo();
		void DrawNativeComponentInfo();

	private:
		RefPtr<GameWorld> m_pWorld;
		GameEntityId m_SelectedEntity;
	};
}