#pragma once
#include "Core/Core.h"

namespace Kepler
{
	struct TEditorElements
	{
	public:
		static constexpr usize GMaxTextEditSymbols = 256;

		static void SetMainWindow(class TWindow* Window) { MainWindow = Window; }

		static bool DragFloat1(CStr pLabel, float& OutFloat, float Speed = 1, float Min = 0, float Max = 0);
		static bool DragFloat2(CStr pLabel, float2& OutFloat, float Speed = 1, float Min = 0, float Max = 0);
		static bool DragFloat3(CStr pLabel, float3& OutFloat, float Speed = 1, float Min = 0, float Max = 0);
		static bool DragFloat4(CStr pLabel, float4& OutFloat, float Speed = 1, float Min = 0, float Max = 0);

		static bool Container(CStr pText);
		static bool EditText(CStr pLabel, CStr pInitialText, char(&pOutBuffer)[GMaxTextEditSymbols]);
	private:
		static void CheckWrappingCursor();

	private:
		static class TWindow* MainWindow;
		static float WrappingInterval;
	};
}