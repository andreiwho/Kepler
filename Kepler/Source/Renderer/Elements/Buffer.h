#pragma once
#include "Core/Malloc.h"

namespace Kepler
{
	class TBuffer : public TRefCounted
	{
	public:
		virtual void* GetNativeHandle() const = 0;
	};
}