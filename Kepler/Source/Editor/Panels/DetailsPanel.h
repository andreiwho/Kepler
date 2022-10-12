#pragma once
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	class TEditorDetailsPanel
	{
	public:
		TEditorDetailsPanel(TRef<TGameWorld> pWorld, TGameEntityId entity);
		
		void Draw();

	private:
		void DrawTransformComponentInfo();
		void DrawCameraComponentInfo();
		void DrawMaterialComponentInfo();
		void DrawLightInfo();
		void DrawEntityInfo();

	private:
		TRef<TGameWorld> m_pWorld;
		TGameEntityId m_SelectedEntity;
	};
}