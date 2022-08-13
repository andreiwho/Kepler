#include "Input.h"

#include "Platform/Platform.h"

bool TInput::GetKey(EKeyCode key)
{
	const Kepler::TKeyboardState& keyboard = CHECKED(Kepler::TPlatform::Get())->GetKeyboardState();
	return keyboard.GetKeyState(key);
}

bool TInput::GetMouseButon(EMouseButton button)
{
	const Kepler::TMouseState& mouse = CHECKED(Kepler::TPlatform::Get())->GetMouseState();
	return mouse.GetButtonState(button);
}

void TInput::GetMousePosition(float& outX, float& outY)
{
	const Kepler::TMouseState& mouse = CHECKED(Kepler::TPlatform::Get())->GetMouseState();
	auto position = mouse.GetPosition();
	outX = position.X;
	outY = position.Y;
}
