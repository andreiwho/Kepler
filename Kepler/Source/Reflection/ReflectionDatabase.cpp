#include "Reflection/ReflectionDatabase.h"

namespace ke
{
	ReflectionDatabase* ReflectionDatabase::Instance = nullptr;

	ReflectionDatabase::ReflectionDatabase()
	{
		Instance = this;
	}

	void ReflectionDatabase::PushClass(RefPtr<ReflectedClass> cls)
	{
		if (cls->IsComponentClass())
		{
			m_ComponentClasses.AppendBack(cls->GetClassId());
		}
		m_Classes[typehash64(cls->GetName())] = cls;
	}

}