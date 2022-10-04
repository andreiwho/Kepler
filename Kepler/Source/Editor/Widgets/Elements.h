#pragma once
#include "Core/Core.h"

namespace ke
{
	struct TEditorElements
	{
	public:
		static constexpr usize GMaxTextEditSymbols = 256;

		static void SetMainWindow(class TWindow* pWindow) { s_MainWindow = pWindow; }

		static bool DragFloat1(CStr pLabel, float& outFloat, float speed = 1, float min = 0, float max = 0);
		static bool DragFloat2(CStr pLabel, float2& OutFloat, float Speed = 1, float min = 0, float max = 0);
		static bool DragFloat3(CStr pLabel, float3& OutFloat, float Speed = 1, float min = 0, float max = 0);
		static bool DragFloat4(CStr pLabel, float4& OutFloat, float Speed = 1, float min = 0, float max = 0);

		static bool Container(CStr pText);
		static bool EditText(CStr pLabel, CStr pInitialText, char(&pOutBuffer)[GMaxTextEditSymbols]);

		static bool BeginFieldTable(CStr pLabel, i32 ColumnCount);
		static void EndFieldTable();
		static void NextFieldRow(CStr pLabel);
		static void SetColumn(i32 column);

	private:
		static void CheckWrappingCursor();

	private:
		static class TWindow* s_MainWindow;
		static float s_WrappingInterval;
	};
}