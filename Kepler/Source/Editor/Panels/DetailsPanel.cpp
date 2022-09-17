#include "DetailsPanel.h"
#include "../Widgets/Elements.h"
#include "World/Game/GameEntity.h"
#include "imgui.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Helpers/EntityHelper.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Tools/MaterialLoader.h"

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
				DrawMaterialComponentInfo();

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
			if (TEditorElements::BeginFieldTable("entity", 2))
			{
				TEditorElements::NextFieldRow("Name");
				char NameBuffer[TEditorElements::GMaxTextEditSymbols];
				memset(NameBuffer, 0, sizeof(NameBuffer));
				if (TEditorElements::EditText("Name", EntityRef.GetName().c_str(), NameBuffer))
				{
					NameBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
					EntityRef.SetName(NameBuffer);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawTransformComponentInfo()
	{
		TGameEntity& EntityRef = WorldContext->GetEntityFromId(SelectedEntity);
		if (TEditorElements::Container("TRANSFORM"))
		{
			if (TEditorElements::BeginFieldTable("transform", 2))
			{
				TEditorElements::NextFieldRow("Location");
				auto Location = EntityRef.GetLocation();
				if (TEditorElements::DragFloat3("Location", Location, 0.001f))
				{
					EntityRef.SetLocation(Location);
				}

				TEditorElements::NextFieldRow("Rotation");
				auto Rotation = EntityRef.GetRotation();
				if (TEditorElements::DragFloat3("Rotation", Rotation, 0.1f))
				{
					EntityRef.SetRotation(Rotation);
				}

				TEditorElements::NextFieldRow("Scale");
				auto Scale = EntityRef.GetScale();
				if (TEditorElements::DragFloat3("Scale", Scale, 0.01f))
				{
					EntityRef.SetScale(Scale);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawCameraComponentInfo()
	{
		TCamera& Camera = WorldContext->GetComponent<TCameraComponent>(SelectedEntity).GetCamera();
		if (TEditorElements::Container("CAMERA"))
		{
			if (TEditorElements::BeginFieldTable("camera", 2))
			{
				TEditorElements::NextFieldRow("Field of View");
				auto FieldOfView = Camera.GetFOV();
				if (TEditorElements::DragFloat1("Field Of View", FieldOfView, 0.001f))
				{
					Camera.SetFOV(FieldOfView);
				}

				TEditorElements::NextFieldRow("Near/Far Clip");
				auto FrustumDepth = float2(Camera.GetNearClip(), Camera.GetFarClip());
				if (TEditorElements::DragFloat2("Near/Far Clip", FrustumDepth, 0.1f))
				{
					Camera.SetNearClip(FrustumDepth.x);
					Camera.SetFarClip(FrustumDepth.y);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawMaterialComponentInfo()
	{
		TEntityHandle Entity = TEntityHandle{ WorldContext, SelectedEntity };
		if (!Entity)
		{
			return;
		}

		if (auto pMaterialComponent = Entity.GetComponent<TMaterialComponent>())
		{
			if (TEditorElements::Container("MATERIAL"))
			{
				if (TEditorElements::BeginFieldTable("material", 2))
				{
					TEditorElements::NextFieldRow("Path");
					char PathBuffer[TEditorElements::GMaxTextEditSymbols];
					memset(PathBuffer, 0, sizeof(PathBuffer));
					if (TEditorElements::EditText("Path",
						pMaterialComponent->GetMaterialParentAssetPath().c_str(), 
						PathBuffer))
					{
						PathBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';

						if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(PathBuffer))
						{
							pMaterialComponent->SetMaterial(pMaterial);
						}
					}

					TEditorElements::NextFieldRow("Reload");
					if (ImGui::Button("Reload Material"))
					{
						if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(PathBuffer))
						{
							pMaterialComponent->SetMaterial(pMaterial);
						}
					}

					TEditorElements::EndFieldTable();
				}
			}
		}
	}

}