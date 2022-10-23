#include "Class.h"

namespace ke
{
	ReflectedField::ReflectedField(id64 id, const FieldMetadata& fieldMetadata, GetAccessorType getAccessor, SetAccessorType setAccessor)
		: m_TypeId(id)
		, m_Metadata(fieldMetadata)
		, m_GetAccessor(getAccessor)
		, m_SetAccessor(setAccessor)
	{
		CHECK(getAccessor);
		CHECK(setAccessor);
	}

	void ReflectedClass::PushField(const String& name, ReflectedField&& field)
	{
		CHECK(!m_Fields.Contains(name));
		m_Fields[name] = field;
	}

	void ReflectedEnum::PushEnumValue(const String& name, u32 value)
	{
		m_EnumValues.AppendBack({ name, value });
	}

}