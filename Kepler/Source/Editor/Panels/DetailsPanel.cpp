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
	namespace
	{
		void DrawReflectedField(const String& name, ReflectedField& field, void* pHandler)
		{
			bool bNotBaseType = false;

			// Check metadata
			const auto& md = field.GetMetadata();
			if (md.bHideInDetails)
			{
				return;
			}

			if (md.bReadOnly)
			{
				ImGui::BeginDisabled(true);
			}

			switch (field.GetTypeHash())
			{
			case typehash64("float"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float>(pHandler);
				TEditorElements::DragFloat1(name.c_str(), *value, md.EditSpeed);
			}
			break;
			case typehash64("float2"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float2>(pHandler);
				TEditorElements::DragFloat2(name.c_str(), *value, md.EditSpeed);
			}
			break;
			case typehash64("float3"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float3>(pHandler);
				TEditorElements::DragFloat3(name.c_str(), *value, md.EditSpeed);
			}
			break;
			case typehash64("float4"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<float4>(pHandler);
				TEditorElements::DragFloat4(name.c_str(), *value, md.EditSpeed);
			}
			break;
			case typehash64("bool"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<bool>(pHandler);
				ImGui::Checkbox(name.c_str(), value);
			}
			break;
			case typehash64("String"):
			{
				TEditorElements::NextFieldRow(name.c_str());
				auto value = field.GetValueFor<String>(pHandler);
				char outBuffer[TEditorElements::GMaxTextEditSymbols];
				memset(outBuffer, 0, sizeof(outBuffer));
				if (TEditorElements::EditText(name.c_str(), value->c_str(), outBuffer, md.bReadOnly))
				{
					outBuffer[TEditorElements::GMaxTextEditSymbols - 1] = '\0';
					String newValue = outBuffer;
					field.SetValueFor(pHandler, &newValue);
				}
			}
			break;
			default:
				bNotBaseType = true;
				break;
			}

			// End metadata
			if (field.GetMetadata().bReadOnly)
			{
				ImGui::EndDisabled();
			}

			if (bNotBaseType)
			{
				if (md.bIsEnum)
				{
					auto pEnumClass = ReflectionDatabase::Get()->FindClassByTypeHash(field.GetTypeHash());
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
					ImGui::TableHeader(name.c_str());
					if (RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassByTypeHash(field.GetTypeHash()))
					{
						for (auto& [fieldName, classField] : pClass->GetFields())
						{
							DrawReflectedField(fieldName, classField, field.GetValueFor<void*>(pHandler));
						}
					}
				}
			}
		}
	}

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
			for (const typehash64& id : pNativeComp->GetComponentIds())
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
}