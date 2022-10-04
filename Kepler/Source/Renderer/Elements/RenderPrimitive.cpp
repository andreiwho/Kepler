#include "RenderPrimitive.h"
#include "Core/Malloc.h"

namespace ke
{

	void TRenderPrimitive::AddRef() const
	{
		ReferenceCount++;
	}

	void TRenderPrimitive::Release() const
	{
		ReferenceCount--;
		
	}

}