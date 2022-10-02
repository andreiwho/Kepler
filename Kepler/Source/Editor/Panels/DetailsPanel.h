#pragma once
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace ke
{
	class TEditorDetailsPanel
	{
	public:
		TEditorDetailsPanel(TRef<TGameWorld> InWorldContext, TGameEntityId InSelectedEntity);
		
		void Draw();

	private:
		void DrawTransformComponentInfo();
		void DrawCameraComponentInfo();
		void DrawMaterialComponentInfo();
		void DrawEntityInfo();

	private:
		TRef<TGameWorld> WorldContext;
		TGameEntityId SelectedEntity;
	};
}