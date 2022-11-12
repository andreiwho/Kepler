#include "Delegate.h"

namespace ke
{

	void MulticastDelegate::Unbind(DelegateHandle handle)
	{
		if (m_Callbacks.GetLength() > handle)
		{
			m_Callbacks.Remove(m_Callbacks.begin() + handle);
		}
	}

	void MulticastDelegate::Invoke()
	{
		for (auto& pCallback : m_Callbacks)
		{
			pCallback();
		}
	}
}