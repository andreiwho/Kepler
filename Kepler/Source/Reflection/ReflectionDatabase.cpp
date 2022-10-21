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
		m_Classes[cls->GetName()] = cls;
	}

}