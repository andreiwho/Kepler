#include "Input.h"
#include "Platform/Platform.h"

bool TInput::GetKey(EKeyCode key)
{
	const ke::TKeyboardState& keyboard = CHECKED(ke::TPlatform::Get())->GetKeyboardState();
	return keyboard.GetKeyState(key);
}

bool TInput::GetMouseButon(EMouseButton button)
{
	const ke::TMouseState& mouse = CHECKED(ke::TPlatform::Get())->GetMouseState();
	return mouse.GetButtonState(button);
}

void TInput::GetMousePosition(float& outX, float& outY)
{
	const ke::TMouseState& mouse = CHECKED(ke::TPlatform::Get())->GetMouseState();
	auto position = mouse.GetPosition();
	outX = position.X;
	outY = position.Y;
}
