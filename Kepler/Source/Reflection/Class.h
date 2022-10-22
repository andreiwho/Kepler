#pragma once
#include "Core/Core.h"

namespace ke
{
	class ReflectedField
	{
	public:
		using GetAccessorType = void* (*)(void*);
		using SetAccessorType = void (*)(void*, void*);
		ReflectedField(id64 id, GetAccessorType getAccessor, SetAccessorType setAccessor);
		ReflectedField() = default;

		template<typename GetType, typename HandlerType>
		GetType* GetValueFor(HandlerType* handler)
		{
			return (GetType*)m_GetAccessor((void*)handler);
		}

		template<typename SetType, typename HandlerType>
		void SetValueFor(HandlerType* pHandler, SetType* pValue)
		{
			m_SetAccessor(pHandler, pValue);
		}

	private:
		id64 m_TypeId{0};
		GetAccessorType m_GetAccessor{};
		SetAccessorType m_SetAccessor{};
	};

	class ReflectedClass : public IntrusiveRefCounted
	{
	public:
		virtual String GetName() const = 0;
		virtual bool HasParent() const = 0;
		virtual String GetParentName() const = 0;

		inline const Map<String, ReflectedField>& GetFields() const&
		{
			return m_Fields;
		}

		inline ReflectedField& GetFieldByName(const String& name)
		{
			CHECK(m_Fields.Contains(name));
			return m_Fields[name];
		}

		inline const ReflectedField& GetFieldByName(const String& name) const
		{
			CHECK(m_Fields.Contains(name));
			return m_Fields[name];
		}

	protected:
		void PushField(const String& name, ReflectedField&& field);

	private:
		Map<String, ReflectedField> m_Fields;
	};
}