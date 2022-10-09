#include "DetailsPanel.h"
#include "../Widgets/Elements.h"
#include "World/Game/GameEntity.h"
#include "imgui.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Helpers/EntityHelper.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Tools/MaterialLoader.h"
#include "World/Game/Components/Light/AmbientLightComponent.h"
#include "glm/gtc/type_ptr.inl"

namespace ke
{

	TEditorDetailsPanel::TEditorDetailsPanel(TRef<TGameWorld> pWorld, TGameEntityId selectedEntity)
		: m_pWorld(pWorld)
		, m_SelectedEntity(selectedEntity)
	{
	}

	void TEditorDetailsPanel::Draw()
	{
		// TEMP
		ImGui::Begin("Details");
		{
			if (m_pWorld && m_pWorld->IsValidEntity(m_SelectedEntity))
			{
				DrawEntityInfo();
				DrawTransformComponentInfo();
				DrawMaterialComponentInfo();

				if (m_pWorld->IsCamera(m_SelectedEntity))
				{
					DrawCameraComponentInfo();
				}

				if (m_pWorld->IsLight(m_SelectedEntity))
				{
					DrawLightInfo();
				}
			}
		}
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawEntityInfo()
	{
		TGameEntity& entity = m_pWorld->GetEntityFromId(m_SelectedEntity);
		if (TEditorElements::Container("ENTITY"))
		{
			if (TEditorElements::BeginFieldTable("details", 2))
			{
				TEditorElements::NextFieldRow("Name");
				char nameBuffer[TEditorElements::GMaxTextEditSymbols];
				memset(nameBuffer, 0, sizeof(nameBuffer));
				if (TEditorElements::EditText("Name", entity.GetName().c_str(), nameBuffer))
				{
					nameBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
					entity.SetName(nameBuffer);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawTransformComponentInfo()
	{
		TGameEntity& entity = m_pWorld->GetEntityFromId(m_SelectedEntity);
		if (TEditorElements::Container("TRANSFORM"))
		{
			if (TEditorElements::BeginFieldTable("details", 2))
			{
				TEditorElements::NextFieldRow("Location");
				auto location = entity.GetLocation();
				if (TEditorElements::DragFloat3("Location", location, 0.001f))
				{
					entity.SetLocation(location);
				}

				TEditorElements::NextFieldRow("Rotation");
				auto rotation = entity.GetRotation();
				if (TEditorElements::DragFloat3("Rotation", rotation, 0.1f))
				{
					entity.SetRotation(rotation);
				}

				TEditorElements::NextFieldRow("Scale");
				auto scale = entity.GetScale();
				if (TEditorElements::DragFloat3("Scale", scale, 0.01f))
				{
					entity.SetScale(scale);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawCameraComponentInfo()
	{
		MathCamera& camera = m_pWorld->GetComponent<CameraComponent>(m_SelectedEntity).GetCamera();
		if (TEditorElements::Container("CAMERA"))
		{
			if (TEditorElements::BeginFieldTable("details", 2))
			{
				TEditorElements::NextFieldRow("Field of View");
				auto fov = camera.GetFOV();
				if (TEditorElements::DragFloat1("Field Of View", fov, 0.001f))
				{
					camera.SetFOV(fov);
				}

				TEditorElements::NextFieldRow("Near/Far Clip");
				auto frustomDepth = float2(camera.GetNearClip(), camera.GetFarClip());
				if (TEditorElements::DragFloat2("Near/Far Clip", frustomDepth, 0.1f))
				{
					camera.SetNearClip(frustomDepth.x);
					camera.SetFarClip(frustomDepth.y);
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawMaterialComponentInfo()
	{
		TEntityHandle entity = TEntityHandle{ m_pWorld, m_SelectedEntity };
		if (!entity)
		{
			return;
		}

		if (auto pMaterialComponent = entity.GetComponent<TMaterialComponent>())
		{
			if (TEditorElements::Container("MATERIAL"))
			{
				if (TEditorElements::BeginFieldTable("details", 2))
				{
					TEditorElements::NextFieldRow("Path");
					char pathBuffer[TEditorElements::GMaxTextEditSymbols];
					memset(pathBuffer, 0, sizeof(pathBuffer));
					if (TEditorElements::EditText("Path",
						pMaterialComponent->GetMaterialParentAssetPath().c_str(), 
						pathBuffer))
					{
						pathBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';

						if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(pathBuffer, true))
						{
							pMaterialComponent->SetMaterial(pMaterial);
						}
					}

					TEditorElements::NextFieldRow("Reload");
					if (ImGui::Button("Reload Material"))
					{
						if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(pathBuffer, true))
						{
							pMaterialComponent->SetMaterial(pMaterial);
						}
					}

					TEditorElements::EndFieldTable();
				}
			}
		}
	}

	void TEditorDetailsPanel::DrawLightInfo()
	{
		TEntityHandle entity = TEntityHandle{ m_pWorld, m_SelectedEntity };
		if (!entity)
		{
			return;
		}

		if (auto pAmbientLight = entity.GetComponent<AmbientLightComponent>())
		{
			if (TEditorElements::Container("AMBIENT LIGHT"))
			{
				if (TEditorElements::BeginFieldTable("details", 2))
				{
					TEditorElements::NextFieldRow("Color");

					float3 color = pAmbientLight->GetColor();
					if (TEditorElements::DragFloat3("Ambient Color", color, 0.01f, 0.0f, 100.0f))
					{
						pAmbientLight->SetColor(color);
					}

					TEditorElements::EndFieldTable();
				}
			}
		}
	}

}