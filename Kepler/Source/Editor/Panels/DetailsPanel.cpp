#include "DetailsPanel.h"
#include "../Widgets/Elements.h"
#include "World/Game/GameEntity.h"
#include "imgui.h"
#include "World/Camera/CameraComponent.h"

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

				if (WorldContext->IsCamera(SelectedEntity))
				{
					DrawCameraComponentInfo();
				}
			}
		}
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawEntityInfo()
	{
		TGameEntity& EntityRef = WorldContext->GetEntityFromId(SelectedEntity);
		if (TEditorElements::Container("ENTITY"))
		{
			char NameBuffer[TEditorElements::GMaxTextEditSymbols];
			memset(NameBuffer, 0, sizeof(NameBuffer));
			if (TEditorElements::EditText("Name", EntityRef.GetName().c_str(), NameBuffer))
			{
				NameBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
				EntityRef.SetName(NameBuffer);
			}
			ImGui::NewLine();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawTransformComponentInfo()
	{
		TGameEntity& EntityRef = WorldContext->GetEntityFromId(SelectedEntity);
		if (TEditorElements::Container("TRANSFORM"))
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
			ImGui::NewLine();
		}
	}

	void TEditorDetailsPanel::DrawCameraComponentInfo()
	{
		TCamera& Camera = WorldContext->GetComponent<TCameraComponent>(SelectedEntity).GetCamera();
		
		if (TEditorElements::Container("CAMERA"))
		{
			auto FieldOfView = Camera.GetFOV();
			if (TEditorElements::DragFloat1("Field Of View", FieldOfView, 0.001f))
			{
				Camera.SetFOV(FieldOfView);
			}

			auto FrustumDepth = float2(Camera.GetNearClip(), Camera.GetFarClip());
			if (TEditorElements::DragFloat2("Near/Far Clip", FrustumDepth, 0.1f))
			{
				Camera.SetNearClip(FrustumDepth.x);
				Camera.SetFarClip(FrustumDepth.y);
			}
			ImGui::NewLine();
		}
	}

}