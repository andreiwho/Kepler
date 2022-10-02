#include "Keyboard.h"

namespace ke
{

	void TKeyboardState::OnKeyPressed(EKeyCode code)
	{
		if (code == EKeyCode::Unknown)
		{
			return;
		}
		m_KeyStates[code] = true;
	}

	void TKeyboardState::OnKeyReleased(EKeyCode code)
	{

		if (code == EKeyCode::Unknown)
		{
			return;
		}
		m_KeyStates[code] = false;
	}

}