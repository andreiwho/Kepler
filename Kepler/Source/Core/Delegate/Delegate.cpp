#include "Delegate.h"

namespace ke
{
	void MulticastDelegate::Invoke()
	{
		for (auto& pCallback : m_Callbacks)
		{
			pCallback();
		}
	}
}