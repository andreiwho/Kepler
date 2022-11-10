#include "Elements.h"

#include <imgui.h>
#include "imgui_internal.h"
#include "Platform/Window.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"

namespace ImGui
{
	// This code was stolen from this answer on stack overflow
	// https://github.com/ocornut/imgui/issues/1831#issuecomment-392202912
	// I love it, man
	bool DragFloatN_Colored(const char* label, float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();
		PushID(label);
		ImGui::PushMultiItemsWidths(components, ImGui::GetContentRegionAvail().x);
		for (int i = 0; i < components; i++)
		{
			static const ImU32 colors[] = {
				0xBB0000FF, // red
				0xBB00FF00, // green
				0xBBFF0000, // blue
				0xBBFFFFFF, // white for alpha?
			};

			PushID(i);
			value_changed |= DragFloat("##v", &v[i], v_speed, v_min, v_max, display_format, power);

			const ImVec2 min = GetItemRectMin();
			const ImVec2 max = GetItemRectMax();
			const float spacing = g.Style.FrameRounding;
			const float halfSpacing = spacing / 2;

			// This is the main change
			window->DrawList->AddLine({ min.x + spacing, max.y - halfSpacing }, { max.x - spacing, max.y - halfSpacing }, colors[i], 4);

			SameLine(0, g.Style.ItemInnerSpacing.x);
			PopID();
			PopItemWidth();
		}
		PopID();

		TextUnformatted(label, FindRenderedTextEnd(label));
		EndGroup();

		return value_changed;
	}
}

namespace ke
{

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::DragFloat1(CStr pLabel, float& OutFloat, float Speed, float Min, float Max)
	{
		const bool bValueChanged = ImGui::DragFloatN_Colored(pLabel, &OutFloat, 1, Speed, Min, Max, "%.3f", 1.0f);

		if (bValueChanged)
		{
			CheckWrappingCursor();
		}

		return bValueChanged;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::DragFloat2(CStr pLabel, float2& OutFloat, float Speed, float Min, float Max)
	{
		const bool bValueChanged = ImGui::DragFloatN_Colored(pLabel, &OutFloat.x, 2, Speed, Min, Max, "%.3f", 1.0f);

		if (bValueChanged)
		{
			CheckWrappingCursor();
		}

		return bValueChanged;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::DragFloat3(CStr pLabel, float3& OutFloat, float Speed, float Min, float Max)
	{
		const bool bValueChanged = ImGui::DragFloatN_Colored(pLabel, &OutFloat.x, 3, Speed, Min, Max, "%.3f", 1.0f);

		if (bValueChanged)
		{
			CheckWrappingCursor();
		}

		return bValueChanged;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::DragFloat4(CStr pLabel, float4& OutFloat, float Speed, float Min, float Max)
	{
		const bool bValueChanged = ImGui::DragFloatN_Colored(pLabel, &OutFloat.x, 4, Speed, Min, Max, "%.3f", 1.0f);

		if (bValueChanged)
		{
			CheckWrappingCursor();
		}

		return bValueChanged;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::Container(CStr pText)
	{
		return ImGui::CollapsingHeader(pText, ImGuiTreeNodeFlags_DefaultOpen);
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorElements::EditText(CStr pLabel, CStr pInitialText, char(&pOutBuffer)[GMaxTextEditSymbols], bool bDisabled)
	{
		if (pInitialText)
		{
			strcpy(pOutBuffer, pInitialText);
		}

		u32 flags = ImGuiInputTextFlags_EnterReturnsTrue;
		if (bDisabled)
		{
			flags |= ImGuiInputTextFlags_ReadOnly;
		}

		bool bReturn = ImGui::InputText(fmt::format("##{}", pLabel).c_str(), pOutBuffer, GMaxTextEditSymbols, flags);
		return bReturn;
	}

	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		i32 CurrentFieldId = 0;
	}

	bool TEditorElements::BeginFieldTable(CStr pLabel, i32 ColumnCount)
	{
		bool bBegan = ImGui::BeginTable(pLabel, ColumnCount, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		return bBegan;
	}

	void TEditorElements::DrawReflectedField(const String& name, ReflectedField& field, void* pHandler)
	{

		bool bNotBaseType = false;

		// Check metadata
		const auto& md = field.GetMetadata();
		if (md.bHideInDetails)
		{
			return;
		}

		bool bCanEdit = field.CanEdit(pHandler);
		if (!bCanEdit)
		{
			ImGui::BeginDisabled(true);
		}

		bool bBeganDragDropTarget = false;
		switch (field.GetTypeHash())
		{
		case ClassId("AssetTreeNode"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			AssetTreeNode* pAsset = field.GetValueFor<AssetTreeNode>(pHandler);

			const ImGuiPayload* pPayload = nullptr;

			if (pAsset)
			{
				char outBuffer[TEditorElements::GMaxTextEditSymbols]{};
				TEditorElements::EditText("asset", pAsset->GetPath().c_str(), outBuffer, false);
			}
			else
			{
				char outBuffer[TEditorElements::GMaxTextEditSymbols]{};
				TEditorElements::EditText("asset", "No Asset Defined", outBuffer, false);
			}

			if (md.bEnableDragDrop)
			{
				bBeganDragDropTarget = ImGui::BeginDragDropTarget();
			}

			if (bBeganDragDropTarget)
			{
				StringView assetType{};
				switch (md.FieldAssetType)
				{
					ImGui::EndDragDropTarget();
				case EFieldAssetType::Material:
					assetType = "MATERIAL";
					break;
				case EFieldAssetType::StaticMesh:
					assetType = "STATICMESH";
					break;
				case EFieldAssetType::Map:
					assetType = "MAP";
					break;
				case EFieldAssetType::Shader:
					assetType = "SHADER";
					break;
				case EFieldAssetType::None:
				default:
					ImGui::EndDragDropTarget();
					break;
				}

				if (pPayload = ImGui::AcceptDragDropPayload(assetType.data()))
				{
					if (AssetTreeNode** ppData = (AssetTreeNode**)pPayload->Data)
					{
						field.SetValueFor(pHandler, *ppData);
					}
				}
			}
		}
		break;
		case ClassId("float"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			auto value = field.GetValueFor<float>(pHandler);
			TEditorElements::DragFloat1(name.c_str(), *value, md.EditSpeed, md.ClampMin, md.ClampMax);
		}
		break;
		case ClassId("float2"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			auto value = field.GetValueFor<float2>(pHandler);
			TEditorElements::DragFloat2(name.c_str(), *value, md.EditSpeed, md.ClampMin, md.ClampMax);
		}
		break;
		case ClassId("float3"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			auto value = field.GetValueFor<float3>(pHandler);
			TEditorElements::DragFloat3(name.c_str(), *value, md.EditSpeed, md.ClampMin, md.ClampMax);
		}
		break;
		case ClassId("float4"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			auto value = field.GetValueFor<float4>(pHandler);
			TEditorElements::DragFloat4(name.c_str(), *value, md.EditSpeed, md.ClampMin, md.ClampMax);
		}
		break;
		case ClassId("bool"):
		{
			TEditorElements::NextFieldRow(name.c_str());
			auto value = field.GetValueFor<bool>(pHandler);
			ImGui::Checkbox(fmt::format("##{}", name).c_str(), value);
		}
		break;
		case ClassId("String"):
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
		if (!bCanEdit)
		{
			ImGui::EndDisabled();
		}

		if (bBeganDragDropTarget)
		{
			ImGui::EndDragDropTarget();
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
					i32* pair = field.GetValueFor<i32>(pHandler);
					if (*pair > values.GetLength())
					{
						return;
					}
					auto& selectedValue = values[*pair];
					if (ImGui::BeginCombo(fmt::format("#{}", name).c_str(), selectedValue.c_str()))
					{
						for (auto& [index, string] : values)
						{
							i32 newValue = index;
							if (ImGui::Selectable(string.c_str()))
							{
								field.SetValueFor(pHandler, &newValue);
							}
						}
						ImGui::EndCombo();
					}
				}
			}
			else
			{
				TEditorElements::NextFieldRow(name.c_str());
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

	void TEditorElements::DrawReflectedObjectFields(const String& label, ClassId typeHash, void* pHandler)
	{
		RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassByTypeHash(typeHash);
		if (!pClass)
		{
			return;
		}

		if (TEditorElements::Container(label.c_str()))
		{
			if (TEditorElements::BeginFieldTable("details", 2))
			{
				for (auto& [name, field] : pClass->GetFields())
				{
					DrawReflectedField(name, field, pHandler);
				}

				TEditorElements::EndFieldTable();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorElements::EndFieldTable()
	{
		ImGui::EndTable();
		CurrentFieldId = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorElements::NextFieldRow(CStr pLabel)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(pLabel);
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorElements::SetColumn(i32 Column)
	{
		ImGui::TableSetColumnIndex(Column);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorElements::CheckWrappingCursor()
	{
		ImGuiIO& IO = ImGui::GetIO();
		if (IO.MousePos.x >= IO.DisplaySize.x - s_WrappingInterval)
		{
			// Wrap mouse cursor
			float2 NewPos = float2(s_WrappingInterval + 1, IO.MousePos.y);
			s_MainWindow->SetCursorPosition(NewPos);
			IO.MousePos = { NewPos.x, NewPos.y };
			IO.MousePosPrev = { NewPos.x, NewPos.y };
		}
		if (IO.MousePos.x <= s_WrappingInterval)
		{
			// Wrap mouse cursor
			float2 NewPos = float2(IO.DisplaySize.x - s_WrappingInterval - 1, IO.MousePos.y);
			s_MainWindow->SetCursorPosition(NewPos);
			IO.MousePos = { NewPos.x, NewPos.y };
			IO.MousePosPrev = { NewPos.x, NewPos.y };
		}
	}


	//////////////////////////////////////////////////////////////////////////
	class TWindow* TEditorElements::s_MainWindow;
	float TEditorElements::s_WrappingInterval = 20.0f;
}