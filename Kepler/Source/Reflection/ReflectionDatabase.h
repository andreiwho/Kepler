#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Containers/DynArray.h"
#include "Core/Containers/ChaosMap.h"
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

		RefPtr<ReflectedClass> FindClassByTypeHash(ClassId id)
		{
			if (m_Classes.Contains(id))
			{
				return m_Classes[id];
			}
			return nullptr;
		}

		inline const Array<ClassId>& GetComponentClasses() const { return m_ComponentClasses; }

	private:
		Map<ClassId, RefPtr<ReflectedClass>> m_Classes;
		Array<ClassId> m_ComponentClasses;
	};

	template<typename T>
	static RefPtr<ReflectedClass> GetReflectedClass()
	{
		return ReflectionDatabase::Get()->GetClass<T>();
	}

	static RefPtr<ReflectedClass> GetReflectedClass(ClassId typeHash)
	{
		return ReflectionDatabase::Get()->FindClassByTypeHash(typeHash);
	}

	template<typename T>
	static RefPtr<ReflectedEnum> GetReflectedEnum()
	{
		auto pEnum = GetReflectedClass<T>();
		if (pEnum->IsEnum())
		{
			return RefCast<ReflectedEnum>(pEnum);
		}
		return nullptr;
	}

	static RefPtr<ReflectedEnum> GetReflectedEnum(ClassId typeHash)
	{
		auto pEnum = GetReflectedClass(typeHash);
		if (pEnum->IsEnum())
		{
			return RefCast<ReflectedEnum>(pEnum);
		}
		return nullptr;
	}

	template<typename EnumType>
	static String EnumValueToString(EnumType value)
	{
		auto pEnum = GetReflectedEnum<EnumType>();
		CHECK(pEnum);
		return pEnum->ValueToString((i32)value);
	}

	static String EnumValueToString(ClassId typeHash, i32 value)
	{
		auto pEnum = GetReflectedEnum(typeHash);
		CHECK(pEnum);
		return pEnum->ValueToString(value);
	}
}