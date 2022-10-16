#include "RenderPrimitive.h"
#include "Core/Malloc.h"

namespace ke
{

	void IRenderPrimitive::AddRef() const
	{
		m_ReferenceCount++;
	}

	void IRenderPrimitive::Release() const
	{
		m_ReferenceCount--;
		
	}

}