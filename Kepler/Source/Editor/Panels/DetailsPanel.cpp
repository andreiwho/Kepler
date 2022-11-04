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
#include "World/Game/Components/Light/DirectionalLightComponent.h"
#include "World/Scripting/NativeComponentContainer.h"

namespace ke
{
	EntityDetailsPanel::EntityDetailsPanel(RefPtr<GameWorld> pWorld, GameEntityId selectedEntity)
		: m_pWorld(pWorld)
		, m_SelectedEntity(selectedEntity)
	{
	}

	void EntityDetailsPanel::Draw()
	{
		// TEMP
		if(ImGui::Begin("Details"))
		{
			if (m_pWorld && m_pWorld->IsValidEntity(m_SelectedEntity))
			{
				DrawEntityInfo();
				DrawTransformComponentInfo();
				DrawMaterialComponentInfo();
				DrawNativeComponentInfo();

				if (ImGui::Button("Add Component"))
				{
					ImGui::OpenPopup("addcomponent");
				}
				DrawAddComponentPopup();
			}
			ImGui::End();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityDetailsPanel::DrawEntityInfo()
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

	void EntityDetailsPanel::DrawNativeComponentInfo()
	{
		KEPLER_PROFILE_SCOPE();

		EntityHandle entity = { m_pWorld, m_SelectedEntity };
		if (auto pNativeComp = entity.GetComponent<NativeComponentContainer>())
		{
			for (const ClassId& id : pNativeComp->GetComponentIds())
			{
				auto pNativeComponent = m_pWorld->GetComponentById(id, m_SelectedEntity);
				if (!pNativeComponent)
				{
					return;
				}

				RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassByTypeHash(id);
				if (!pClass)
				{
					return;
				}

				if (pClass->GetMetadata().bHideInDetails)
				{
					continue;
				}

				const String& filteredName = SplitAndCapitalizeComponentName(pClass->GetName());
				TEditorElements::DrawReflectedObjectFields(filteredName, id, pNativeComponent);
			}
		}
	}

	void EntityDetailsPanel::DrawAddComponentPopup()
	{
		if (ImGui::BeginPopup("addcomponent"))
		{
			for (ClassId componentHash : ReflectionDatabase::Get()->GetComponentClasses())
			{
				if (RefPtr<ReflectedClass> pClass = GetReflectedClass(componentHash))
				{
					if (ImGui::Selectable(pClass->GetName().c_str()))
					{
						m_pWorld->AddComponentByTypeHash(m_SelectedEntity, componentHash);
					}
				}
			}

			ImGui::EndPopup();
		}
	}

	const ke::String& EntityDetailsPanel::SplitAndCapitalizeComponentName(const String& originalName)
	{
		KEPLER_PROFILE_SCOPE();
		if (m_FilteredComponentNames.Contains(originalName))
		{
			return m_FilteredComponentNames[originalName];
		}

		String outString;
		String token;
		for (const char symbol : originalName)
		{
			if (isupper(symbol))
			{
				if (!token.empty())
				{
					outString += fmt::format("{} ", token);
					token.clear();
				}
			}

			token += toupper(symbol);
		}

		if (!token.empty())
		{
			outString += fmt::format("{} ", token);
		}

		m_FilteredComponentNames[originalName] = outString;
		return m_FilteredComponentNames[originalName];
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityDetailsPanel::DrawTransformComponentInfo()
	{
		KEPLER_PROFILE_SCOPE();
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
	void EntityDetailsPanel::DrawMaterialComponentInfo()
	{
		KEPLER_PROFILE_SCOPE();
		EntityHandle entity = EntityHandle{ m_pWorld, m_SelectedEntity };
		if (!entity)
		{
			return;
		}

		if (auto pMaterialComponent = entity.GetComponent<MaterialComponent>())
		{
			if (TEditorElements::Container("MATERIAL"))
			{
				if (TEditorElements::BeginFieldTable("details", 2))
				{
					auto pClass = ReflectionDatabase::Get()->GetClass<MaterialComponent>();
					if (pClass)
					{
						for (auto& [name, field] : pClass->GetFields())
						{
							TEditorElements::DrawReflectedField(name, field, pMaterialComponent);
						}
					}

					TEditorElements::NextFieldRow("Reload");
					if (ImGui::Button("Reload Material"))
					{
						if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(pMaterialComponent->MaterialAssetPath, true))
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