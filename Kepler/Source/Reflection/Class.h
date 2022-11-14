#pragma once
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Containers/DynArray.h"
#include "Core/Containers/ChaosMap.h"
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
		Shader,
		Texture,
		StaticMesh
	};

	static StringView GetFieldAssetTypePayloadName(EFieldAssetType assetType)
	{
		switch (assetType)
		{
		case ke::EFieldAssetType::None:
			break;
		case ke::EFieldAssetType::All:
			break;
		case ke::EFieldAssetType::Map:
			return "MAP";
			break;
		case ke::EFieldAssetType::Material:
			return "MATERIAL";
			break;
		case ke::EFieldAssetType::StaticMesh:
			return "STATICMESH";
			break;
		case EFieldAssetType::Shader:
			return "SHADER";
			break;
		default:
			break;
		}
		return "";
	}

	struct FieldMetadata
	{
		bool(*EditConditionAccessor)(void* pObject) = nullptr;

		EFieldAssetType FieldAssetType{ EFieldAssetType::None };
		float EditSpeed = 1.0f;
		float ClampMin = -FLT_MIN / INT_MIN;
		float ClampMax = FLT_MAX / INT_MAX;

		bool bReadOnly : 1 = false;
		bool bIsPointer : 1 = false;
		bool bIsRefPtr : 1 = false;
		bool bIsEnum : 1 = false;
		bool bHideInDetails : 1 = false;
		bool bEnableDragDrop : 1 = false;
		bool bHasEditCondition : 1 = false;
	};

	struct ClassMetadata
	{
		bool bHideInDetails : 1 = false;
	};

	static constexpr usize GMaxNumericTypeSize = sizeof(float4);

	template<typename T, typename ... Args>
	constexpr bool GOneOfTypes = (std::is_same_v<T, Args> || ...);
	
	template<typename T>
	constexpr bool GIsNumericType = std::is_arithmetic_v<T> || std::is_enum_v<T> || GOneOfTypes<T, float2, float3, float4, int2, int3, int4, uint2, uint3, uint4, UUID>;

	template<typename T>
	concept NumericValue = GIsNumericType<T>;

	template<typename T>
	concept NotNumericValue = !GIsNumericType<T>;

	struct FieldAny
	{
		FieldAny(void* pPtr) : Pointer(pPtr), bIsPointer(true) {}

		template<NumericValue T>
		FieldAny(T val)
			:	bIsPointer(false)
		{
			memcpy(Numeric, &val, sizeof(val));
		}

		template<typename T>
		auto GetValue() const
		{
			if constexpr (GIsNumericType<T>)
			{
				T numericVal;
				memcpy(&numericVal, Numeric, sizeof(T));
				return numericVal;
			}
			else
			{
				return (T*)Pointer;
			}
		}

		union
		{
			ubyte Numeric[GMaxNumericTypeSize];
			void* Pointer;
		};

		bool bIsPointer = false;
	};

	class ReflectedField
	{
	public:
		using GetAccessorType = FieldAny (*)(void*);
		using SetAccessorType = void (*)(void*, FieldAny);
		ReflectedField(ClassId id, const FieldMetadata& fieldMetadata, GetAccessorType getAccessor, SetAccessorType setAccessor);
		ReflectedField() = default;

		template<typename GetType, typename HandlerType>
		auto GetValueFor(HandlerType* handler)
		{
			return m_GetAccessor((void*)handler).GetValue<GetType>();	
		}

		template<typename SetType, typename HandlerType>
		void SetValueFor(HandlerType* pHandler, SetType pValue)
		{
			if (m_Metadata.bReadOnly)
			{
				return;
			}

			m_SetAccessor(pHandler, FieldAny(pValue));
		}

		inline ClassId GetTypeHash() const
		{
			return m_TypeId;
		}

		const FieldMetadata& GetMetadata() const { return m_Metadata; }

		bool CanEdit(void* pObject) const;

	private:
		ClassId m_TypeId{0};
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

		inline ClassId GetClassId() const { return m_ClassId; }

		inline const ClassMetadata& GetMetadata() const { return m_Metadata; }

		virtual void* RegistryConstruct(entt::entity id, entt::registry& registry) const = 0;
		virtual void RegistryDestroy(entt::entity id, entt::registry& registry) const = 0;
		virtual bool RegistryContains(entt::entity id, entt::registry& registry) const = 0;

	protected:
		ClassId m_ClassId;
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

#define REFL_PASTE_2(a,b,c) a##b##c
#define REFL_PASTE_UNIQUE(a, b, c) REFL_PASTE_2(a,b,c)
#define reflected_body() REFL_PASTE_UNIQUE(FILEID,__LINE__,REFL);
