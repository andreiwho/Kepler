#pragma once
#include "Core/Core.h"
#include "Reflection/Class.h"

namespace ke
{
	holding_reflection_data class ReflectionDatabase
	{
		static ReflectionDatabase* Instance;
	public:
		static ReflectionDatabase* Get()
		{
			return Instance;
		}
		ReflectionDatabase();

		void FillReflectionDatabaseEntries();
		void PushClass(RefPtr<ReflectedClass> cls);

		template<typename T>
		RefPtr<ReflectedClass> GetClass();

		RefPtr<ReflectedClass> FindClassByTypeHash(typehash64 id)
		{
			if (m_Classes.Contains(id))
			{
				return m_Classes[id];
			}
			return nullptr;
		}

	private:
		Map<typehash64, RefPtr<ReflectedClass>> m_Classes;
	};

	template<typename T>
	static RefPtr<ReflectedClass> GetReflectedClass()
	{
		return ReflectionDatabase::Get()->GetClass<T>();
	}
}