#include "RenderPrimitive.h"
#include "Core/Malloc.h"

namespace Kepler
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