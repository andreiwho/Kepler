#include "DetailsPanel.h"
#include "../Widgets/Elements.h"
#include "World/Game/GameEntity.h"
#include "imgui.h"

namespace Kepler
{

	TEditorDetailsPanel::TEditorDetailsPanel(TRef<TGameWorld> InWorldContext, TGameEntityId InSelectedEntity)
		: WorldContext(InWorldContext)
		, SelectedEntity(InSelectedEntity)
	{
	}

	void TEditorDetailsPanel::Draw()
	{
		// TEMP
		ImGui::Begin("Details");
		{
			if (WorldContext && WorldContext->IsValidEntity(SelectedEntity))
			{
				DrawEntityInfo();
				DrawTransformComponentInfo();
			}
		}
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawEntityInfo()
	{
		TGameEntity& EntityRef = WorldContext->GetEntityFromId(SelectedEntity);
		if (TEditorElements::Container("Entity"))
		{
			char NameBuffer[TEditorElements::GMaxTextEditSymbols];
			memset(NameBuffer, 0, sizeof(NameBuffer));
			if (TEditorElements::EditText("Name", EntityRef.GetName().c_str(), NameBuffer))
			{
				NameBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
				EntityRef.SetName(NameBuffer);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawTransformComponentInfo()
	{
		TGameEntity& EntityRef = WorldContext->GetEntityFromId(SelectedEntity);
		if (TEditorElements::Container("Transform"))
		{
			auto Location = EntityRef.GetLocation();
			if (TEditorElements::DragFloat3("Location", Location, 0.001f))
			{
				EntityRef.SetLocation(Location);
			}

			auto Rotation = EntityRef.GetRotation();
			if (TEditorElements::DragFloat3("Rotation", Rotation, 0.1f))
			{
				EntityRef.SetRotation(Rotation);
			}

			auto Scale = EntityRef.GetScale();
			if (TEditorElements::DragFloat3("Scale", Scale, 0.01f))
			{
				EntityRef.SetScale(Scale);
			}
		}
	}

}