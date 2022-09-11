#include "Mouse.h"

namespace Kepler
{

	void TMouseState::OnButtonPressed(EMouseButton button)
	{
		if (button & EMouseButton::Unknown)
		{
			return;
		}
		ButtonStateMask |= button;
	}

	void TMouseState::OnButtonReleased(EMouseButton button)
	{
		if (button & EMouseButton::Unknown)
		{
			return;
		}
		ButtonStateMask &= ~button;
	}

	void TMouseState::OnMouseMove(TMouseVector2f newPosition)
	{
		const TMouseVector2f oldPos = Position;
		Position = newPosition;
		Offset = {Position.X - oldPos.X, Position.Y - oldPos.Y};
	}

	bool TMouseState::GetButtonState(EMouseButton button) const
	{
		if (button & EMouseButton::Unknown)
		{
			return false;
		}
		return ButtonStateMask & button;
	}

	void TMouseState::OnUpdate()
	{
		OldOffset = Offset;
		Offset = TMouseVector2f(0.0f, 0.0f);
	}

}