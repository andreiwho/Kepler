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
#include "World/Scripting/NativeScriptContainer.h"

namespace ke
{
	namespace
	{
		void DrawReflectedField(const String& name, ReflectedField& field, void* pHandler)
		{	
			bool bNotBaseType = false;

			// Check metadata
			if (field.GetMetadata().bReadOnly)
			{
				ImGui::BeginDisabled(true);
			}
			else if (field.GetTypeId() == id64("float"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float>(pHandler);
				TEditorElements::DragFloat1(name.c_str(), *value);
			}
			else if (field.GetTypeId() == id64("float2"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float2>(pHandler);
				TEditorElements::DragFloat2(name.c_str(), *value);
			}
			else if (field.GetTypeId() == id64("float3"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float3>(pHandler);
				TEditorElements::DragFloat3(name.c_str(), *value);
			}
			else if (field.GetTypeId() == id64("float4"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float4>(pHandler);
				TEditorElements::DragFloat4(name.c_str(), *value);
			}

			else if (field.GetTypeId() == id64("bool"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<bool>(pHandler);
				ImGui::Checkbox(name.c_str(), value);
			}
			else if (field.GetTypeId() == id64("String"))
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<String>(pHandler);
				char outBuffer[TEditorElements::GMaxTextEditSymbols];
				memset(outBuffer, 0, sizeof(outBuffer));
				if (TEditorElements::EditText(name.c_str(), value->c_str(), outBuffer, field.GetMetadata().bReadOnly))
				{
					outBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
					String newValue = outBuffer;
					field.SetValueFor(pHandler, &newValue);
				}
			}
			else if (field.GetMetadata().bIsEnum)
			{
				auto pEnumClass = ReflectionDatabase::Get()->FindClassById(field.GetTypeId());
				if (!pEnumClass)
				{
					return;
				}

				if (auto pMyEnum = RefCast<ReflectedEnum>(pEnumClass))
				{
					TEditorElements::NextFieldRow(name.c_str());
					auto& values = pMyEnum->GetEnumValues();
					u32* value = field.GetValueFor<u32>(pHandler);
					auto& selectedValue = values[*value].first;
					if (ImGui::BeginCombo(fmt::format("#{}", name).c_str(), selectedValue.c_str()))
					{
						for (auto& [title, index] : values)
						{
							if (ImGui::Selectable(title.c_str()))
							{
								*value = index;
							}
						}
						ImGui::EndCombo();
					}
				}
			}
			else
			{
				bNotBaseType = true;
			}

			// End metadata
			if (field.GetMetadata().bReadOnly)
			{
				ImGui::EndDisabled();
			}

			if (bNotBaseType)
			{
				ImGui::TableHeader(name.c_str());
				if (RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassById(field.GetTypeId()))
				{
					for (auto& [fieldName, classField] : pClass->GetFields())
					{
						DrawReflectedField(fieldName, classField, field.GetValueFor<void*>(pHandler));
					}
				}
			}
		}
	}

	TEditorDetailsPanel::TEditorDetailsPanel(RefPtr<GameWorld> pWorld, GameEntityId selectedEntity)
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
				DrawNativeComponentInfo();
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

	void TEditorDetailsPanel::DrawNativeComponentInfo()
	{
		KEPLER_PROFILE_SCOPE();

		EntityHandle entity = { m_pWorld, m_SelectedEntity };
		if (auto pNativeComp = entity.GetComponent<NativeScriptContainerComponent>())
		{
			for (const id64& id : pNativeComp->GetComponentIds())
			{
				auto pNativeComponent = m_pWorld->GetNativeComponentById(id, m_SelectedEntity);
				if (!pNativeComponent)
				{
					return;
				}

				RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassById(id);
				if (!pClass)
				{
					return;
				}

				if (TEditorElements::Container(pClass->GetName().c_str()))
				{
					if (TEditorElements::BeginFieldTable("details", 2))
					{
						for (auto& [name, field] : pClass->GetFields())
						{
							DrawReflectedField(name, field, pNativeComponent);
						}

						TEditorElements::EndFieldTable();
					}
				}
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
		EntityHandle handle{ m_pWorld, m_SelectedEntity };
		// MathCamera& camera = m_pWorld->GetComponent<CameraComponent>(m_SelectedEntity).GetCamera();
		if (TEditorElements::Container("CAMERA"))
		{
			if (TEditorElements::BeginFieldTable("details", 2))
			{
				auto pClass = ReflectionDatabase::Get()->GetClass<CameraComponent>();
				if (pClass)
				{
					for (auto& [name, field] : pClass->GetFields())
					{
						DrawReflectedField(name, field, handle.GetComponent<CameraComponent>());
					}
				}
				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorDetailsPanel::DrawMaterialComponentInfo()
	{
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
							DrawReflectedField(name, field, pMaterialComponent);
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

	void TEditorDetailsPanel::DrawLightInfo()
	{
		EntityHandle entity = EntityHandle{ m_pWorld, m_SelectedEntity };
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

		if (auto pDirLight = entity.GetComponent<DirectionalLightComponent>())
		{
			if (TEditorElements::Container("DIRECTIONAL LIGHT"))
			{
				if (TEditorElements::BeginFieldTable("details", 2))
				{
					TEditorElements::NextFieldRow("Color");
					float3 color = pDirLight->GetColor();
					if (TEditorElements::DragFloat3("Color", color, 0.01f, 0.0f, 100.0f))
					{
						pDirLight->SetColor(color);
					}


					TEditorElements::NextFieldRow("Intensity");
					float intensity = pDirLight->GetIntensity();
					if (TEditorElements::DragFloat1("Intensity", intensity, 0.01f, 0.0f, 100.0f))
					{
						pDirLight->SetIntensity(intensity);
					}



					TEditorElements::EndFieldTable();
				}
			}
		}
	}

}