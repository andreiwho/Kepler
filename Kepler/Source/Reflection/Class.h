#pragma once
#include "Core/Core.h"

namespace ke
{
	class ReflectedClass : public IntrusiveRefCounted
	{
	public:
		virtual String GetName() const = 0;
		virtual bool HasParent() const = 0;
		virtual String GetParentName() const = 0;
	};
}