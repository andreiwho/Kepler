#pragma once
#include "Core/Types.h"
#include "Core/Macros.h"

#include "Mouse.h"
#include "Keyboard.h"

class TInput
{
public:
	static bool GetKey(EKeyCode key);
	static bool GetMouseButon(EMouseButton button);
	static void GetMousePosition(float& outX, float& outY);
	static ke::float2 GetMousePosition();
};