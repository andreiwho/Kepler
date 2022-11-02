#pragma once
#include "Core/Core.h"

#include <variant>

namespace ke
{
	using JsonSerializableType = std::variant<
		String,
		float,
		float2,
		float3,
		float4,
		bool,
		i32,
		int2,
		int3,
		int4,
		u32,
		uint2,
		uint3,
		uint4,
		UUID,
		u64,
		std::monostate
	>;

	class JsonObject : public IntrusiveRefCounted
	{
		friend class JsonSerializer;

	public:
		JsonObject(String key, ClassId typeHash) : m_Key(key), m_TypeHash(typeHash) {}

		template<typename T>
		inline decltype(auto) GetValueAs() const
		{
			return std::get<T>(m_Value);
		}

		template<typename T>
		inline bool IsA() const
		{
			return std::holds_alternative<T>(m_Value);
		}

		inline bool IsStructuredType() const
		{
			return IsA<std::monostate>();
		}

		inline const String& GetKey() const
		{
			return m_Key;
		}

		JsonObject& CreateObject(const String& key, ClassId typeHash);
		void Write(JsonSerializableType&& value);
		JsonObject& SerializeSubObject(const String& key, ClassId typeHash, const JsonSerializableType& value);
		JsonObject& SerializeReflectedObject(const String& key, ClassId typeHash, const void* pObject);

		inline bool IsRoot() const
		{
			return !m_Parent;
		}

		inline const Array<RefPtr<JsonObject>>& GetChildren() const
		{
			return m_Children;
		}

		inline ClassId GetTypeHash() const { return m_TypeHash; }
		inline void SetTypeHash(ClassId newTypeHash) { m_TypeHash = newTypeHash; }

	private:
		String m_Key{};
		ClassId m_TypeHash{};
		JsonSerializableType m_Value{ std::monostate{} };
		JsonObject* m_Parent{};
		Array<RefPtr<JsonObject>> m_Children{};
	};

	class JsonSerializer
	{
	public:
		JsonSerializer() = default;
		JsonSerializer(RefPtr<JsonObject> pRootNode) : m_RootNode(pRootNode) {}

		JsonObject& CreateRootObject(const String& key);
		String ToString() const;

	private:
		RefPtr<JsonObject> m_RootNode{};
	};

	class JsonDeserializer
	{
	public:
		JsonDeserializer(const String& text);

		inline RefPtr<JsonObject> GetRootNode() const 
		{
			return m_RootNode;
		}

	private:
		RefPtr<JsonObject> m_RootNode;
	};
}