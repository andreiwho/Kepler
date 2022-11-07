#include "Class.h"

namespace ke
{
	ReflectedField::ReflectedField(ClassId id, const FieldMetadata& fieldMetadata, GetAccessorType getAccessor, SetAccessorType setAccessor)
		: m_TypeId(id)
		, m_Metadata(fieldMetadata)
		, m_GetAccessor(getAccessor)
		, m_SetAccessor(setAccessor)
	{
		CHECK(getAccessor);
		CHECK(setAccessor);
	}

	bool ReflectedField::CanEdit(void* pObject) const
	{
		if (!m_Metadata.bHasEditCondition && !m_Metadata.bReadOnly)
		{
			return true;
		}
		return m_Metadata.bHasEditCondition ? m_Metadata.EditConditionAccessor(pObject) : false;
	}

	void ReflectedClass::PushField(const String& name, ReflectedField&& field)
	{
		CHECK(!m_Fields.Contains(name));
		m_Fields[name] = field;
	}

	void ReflectedEnum::PushEnumValue(const String& name, i32 value)
	{
		m_EnumValues.Insert(value, name);
	}

}