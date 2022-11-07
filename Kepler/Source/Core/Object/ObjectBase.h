#pragma once
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Reflection/ReflectionDatabase.h"

namespace ke
{
	class ObjectBase
	{
	public:
		virtual ReflectedClass* GetClass() const { return nullptr; }
		
		template<typename T>
		inline bool IsInstanceOf() const { return GetClass() == GetReflectedClass<T>(); }
		inline bool IsInstanceOf(ReflectedClass* pClass) const { return GetClass() == pClass; }
	};
}