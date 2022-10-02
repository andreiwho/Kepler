#include "Mouse.h"

namespace ke
{

	void TMouseState::OnButtonPressed(EMouseButton button)
	{
		if (button & EMouseButton::Unknown)
		{
			return;
		}
		m_ButtonStateMask |= button;
	}

	void TMouseState::OnButtonReleased(EMouseButton button)
	{
		if (button & EMouseButton::Unknown)
		{
			return;
		}
		m_ButtonStateMask &= ~button;
	}

	void TMouseState::OnMouseMove(TMouseVector2f newPosition)
	{
		const TMouseVector2f oldPos = m_Position;
		m_Position = newPosition;
		m_Offset = {m_Position.X - oldPos.X, m_Position.Y - oldPos.Y};
	}

	bool TMouseState::GetButtonState(EMouseButton button) const
	{
		if (button & EMouseButton::Unknown)
		{
			return false;
		}
		return m_ButtonStateMask & button;
	}

	void TMouseState::OnUpdate()
	{
		m_OldOffset = m_Offset;
		m_Offset = TMouseVector2f(0.0f, 0.0f);
	}

}