#include "Keyboard.h"

namespace Kepler
{

	void TKeyboardState::OnKeyPressed(EKeyCode code)
	{
		if (code == EKeyCode::Unknown)
		{
			return;
		}
		KeyStates[code] = true;
	}

	void TKeyboardState::OnKeyReleased(EKeyCode code)
	{

		if (code == EKeyCode::Unknown)
		{
			return;
		}
		KeyStates[code] = false;
	}

}