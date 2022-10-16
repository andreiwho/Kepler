#pragma once
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	class TEditorDetailsPanel
	{
	public:
		TEditorDetailsPanel(RefPtr<GameWorld> pWorld, TGameEntityId entity);
		
		void Draw();

	private:
		void DrawTransformComponentInfo();
		void DrawCameraComponentInfo();
		void DrawMaterialComponentInfo();
		void DrawLightInfo();
		void DrawEntityInfo();

	private:
		RefPtr<GameWorld> m_pWorld;
		TGameEntityId m_SelectedEntity;
	};
}