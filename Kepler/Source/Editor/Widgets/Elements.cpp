#include "Elements.h"

#include <imgui.h>
#include "imgui_internal.h"
#include "Platform/Window.h"

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

namespace Kepler
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
	bool TEditorElements::EditText(CStr pLabel, CStr pInitialText, char(&pOutBuffer)[GMaxTextEditSymbols])
	{
		if (pInitialText)
		{
			strcpy(pOutBuffer, pInitialText);
		}

		bool bReturn = ImGui::InputText("##v", pOutBuffer, GMaxTextEditSymbols, ImGuiInputTextFlags_EnterReturnsTrue);
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
		ImGui::PushID(0);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		return bBegan;
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
		ImGui::PushID(CurrentFieldId++);
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
		if (IO.MousePos.x >= IO.DisplaySize.x - WrappingInterval)
		{
			// Wrap mouse cursor
			float2 NewPos = float2(WrappingInterval + 1, IO.MousePos.y);
			MainWindow->SetCursorPosition(NewPos);
			IO.MousePos = { NewPos.x, NewPos.y };
			IO.MousePosPrev = { NewPos.x, NewPos.y };
		}
		if (IO.MousePos.x <= WrappingInterval)
		{
			// Wrap mouse cursor
			float2 NewPos = float2(IO.DisplaySize.x - WrappingInterval - 1, IO.MousePos.y);
			MainWindow->SetCursorPosition(NewPos);
			IO.MousePos = { NewPos.x, NewPos.y };
			IO.MousePosPrev = { NewPos.x, NewPos.y };
		}
	}


	//////////////////////////////////////////////////////////////////////////
	class TWindow* TEditorElements::MainWindow;
	float TEditorElements::WrappingInterval = 20.0f;
}