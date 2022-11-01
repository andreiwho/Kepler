#pragma once
#include "Core/Core.h"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

namespace ke
{
	enum class EFieldAssetType
	{
		None,
		All,
		Map,
		Material,
		StaticMesh
	};

	struct FieldMetadata
	{
		EFieldAssetType FieldAssetType{ EFieldAssetType::None };

		bool bReadOnly : 1 = false;
		bool bIsPointer : 1 = false;
		bool bIsRefPtr : 1 = false;
		bool bIsEnum : 1 = false;
		bool bHideInDetails : 1 = false;
		bool bEnableDragDrop : 1 = false;

		float EditSpeed = 1.0f;
	};

	struct ClassMetadata
	{
		bool bHideInDetails : 1 = false;
	};

	class ReflectedField
	{
	public:
		using GetAccessorType = void* (*)(void*);
		using SetAccessorType = void (*)(void*, void*);
		ReflectedField(typehash64 id, const FieldMetadata& fieldMetadata, GetAccessorType getAccessor, SetAccessorType setAccessor);
		ReflectedField() = default;

		template<typename GetType, typename HandlerType>
		GetType* GetValueFor(HandlerType* handler)
		{
			return (GetType*)m_GetAccessor((void*)handler);
		}

		template<typename SetType, typename HandlerType>
		void SetValueFor(HandlerType* pHandler, SetType* pValue)
		{
			if (m_Metadata.bReadOnly)
			{
				return;
			}

			m_SetAccessor(pHandler, pValue);
		}

		inline typehash64 GetTypeHash() const
		{
			return m_TypeId;
		}

		const FieldMetadata& GetMetadata() const { return m_Metadata; }

	private:
		typehash64 m_TypeId{0};
		FieldMetadata m_Metadata;
		GetAccessorType m_GetAccessor{};
		SetAccessorType m_SetAccessor{};
	};

	class ReflectedClass : public IntrusiveRefCounted
	{
	public:
		virtual String GetName() const = 0;
		virtual bool HasParent() const = 0;
		virtual String GetParentName() const = 0;
		virtual bool IsEnum() const = 0;
		virtual bool IsComponentClass() const = 0;

		inline const Map<String, ReflectedField>& GetFields() const&
		{
			return m_Fields;
		}

		inline Map<String, ReflectedField>& GetFields()&
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

		virtual void* Construct(void* pAddress) const = 0;

		inline typehash64 GetClassId() const { return m_ClassId; }

		inline const ClassMetadata& GetMetadata() const { return m_Metadata; }

		virtual void* RegistryConstruct(entt::entity id, entt::registry& registry) const = 0;

	protected:
		typehash64 m_ClassId;
		void PushField(const String& name, ReflectedField&& field);
		ClassMetadata m_Metadata{};

	private:
		Map<String, ReflectedField> m_Fields;
	};

	class ReflectedEnum : public ReflectedClass
	{
	public:
		inline const auto& GetEnumValues() const& { return m_EnumValues; }
		inline auto& GetEnumValues() & { return m_EnumValues; }

		inline String ValueToString(i32 enumValue) const
		{
			if (m_EnumValues.GetLength() > enumValue)
			{
				return m_EnumValues[enumValue];
			}
			return "No Name";
		}

	protected:
		void PushEnumValue(const String& name, i32 value);

	private:
		Map<i32, String> m_EnumValues;
	};
}