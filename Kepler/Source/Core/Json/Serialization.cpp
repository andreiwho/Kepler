#include "Serialization.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "Reflection/ReflectionDatabase.h"

#ifdef GetObject
#undef GetObject
#endif

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	JsonObject& JsonSerializer::CreateRootObject(const String& key)
	{
		m_RootNode = MakeRef(New<JsonObject>(key, ClassId::none));
		CHECK(m_RootNode);
		return *m_RootNode;
	}

	namespace
	{
		template<glm::length_t Len, typename T, glm::qualifier Q>
		constexpr ClassId GetVectorTypeHash(glm::vec<Len, T, Q>)
		{
			switch (Len)
			{
			case 2:
				if constexpr (std::is_same_v<float, T>) return ClassId("float2");
				if constexpr (std::is_same_v<u32, T>) return ClassId("uint2");
				if constexpr (std::is_same_v<i32, T>) return ClassId("int2");
				break;
			case 3:
				if constexpr (std::is_same_v<float, T>) return ClassId("float3");
				if constexpr (std::is_same_v<u32, T>) return ClassId("uint3");
				if constexpr (std::is_same_v<i32, T>) return ClassId("int3");
				break;
			case 4:
				if constexpr (std::is_same_v<float, T>) return ClassId("float4");
				if constexpr (std::is_same_v<u32, T>) return ClassId("uint4");
				if constexpr (std::is_same_v<i32, T>) return ClassId("int4");
				break;
			default:
				break;
			}
			return 0;
		}

		template<glm::length_t Len, typename T, glm::qualifier Q, typename AllocatorType>
		rapidjson::Value SerializeVectorType(glm::vec<Len, T, Q> vector, AllocatorType& allocator)
		{
			rapidjson::Value value{};

			value.SetObject();
			
			ClassId typeHash = GetVectorTypeHash(vector);
			rapidjson::Value typeHashValue{ typeHash.Value };
			value.AddMember("TypeHash", typeHashValue, allocator);

			rapidjson::Value x{ vector.x };
			rapidjson::Value y{ vector.y };

			value.AddMember("x", x, allocator);
			value.AddMember("y", y, allocator);
			if constexpr (Len == 3)
			{
				rapidjson::Value z{ vector.z };
				value.AddMember("z", z, allocator);
			}
			if constexpr (Len == 4)
			{
				rapidjson::Value w{ vector.w };
				value.AddMember("w", w, allocator);
			}
			return value;
		}

		template<typename AllocatorType>
		void JsonAddMemberRecursive(rapidjson::Value& parent, const JsonObject& object, AllocatorType& allocator)
		{
			rapidjson::Value value;
			if (object.IsA<String>())
			{
				value.SetString(object.GetValueAs<String>().c_str(), allocator);
			}
			else if (object.IsA<UUID>())
			{
				value.SetUint64(object.GetValueAs<UUID>().Value);
			}
			else if (object.IsA<float>())
			{
				value.SetFloat(object.GetValueAs<float>());
			}
			else if (object.IsA<i32>())
			{
				value.SetInt(object.GetValueAs<i32>());
			}
			else if (object.IsA<u32>())
			{
				value = { object.GetValueAs<u32>() };
			}
			else if (object.IsA<bool>())
			{
				value = { object.GetValueAs<bool>() };
			}
			else if (object.IsStructuredType())
			{
				value.SetObject();
				const ClassId typeHash = object.GetTypeHash().Value;
				if (typeHash.Value > 0)
				{
					rapidjson::Value typeHashValue{ object.GetTypeHash().Value };
					value.AddMember("TypeHash", typeHashValue, allocator);
				}

				for (auto& child : object.GetChildren())
				{
					JsonAddMemberRecursive(value, *child, allocator);
				}
			}
#define		SERIALIZE_VEC(Type) else if(object.IsA<Type>()) value = SerializeVectorType(object.GetValueAs<Type>(), allocator)
			SERIALIZE_VEC(float2);
			SERIALIZE_VEC(float3);
			SERIALIZE_VEC(float4);
			SERIALIZE_VEC(int2);
			SERIALIZE_VEC(int3);
			SERIALIZE_VEC(int4);
			SERIALIZE_VEC(uint2);
			SERIALIZE_VEC(uint3);
			SERIALIZE_VEC(uint4);

			if (!value.IsNull())
			{
				String objectKey = object.GetKey();
				rapidjson::Value key{ objectKey.c_str(), allocator };
				parent.AddMember(key, value, allocator);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	String JsonSerializer::ToString() const
	{
		// Create document
		rapidjson::Document outDocument{};
		outDocument.SetObject();
		auto& allocator = outDocument.GetAllocator();

		// Setup the root key
		const String& rootKey = m_RootNode->GetKey();
		rapidjson::GenericStringRef<char> rootName(rootKey.c_str());

		// Setup the root node
		rapidjson::Value root{};
		root.SetObject();

		// Fill in all data
		for (RefPtr<JsonObject> node : m_RootNode->GetChildren())
		{
			JsonAddMemberRecursive(root, *node, allocator);
		}
		outDocument.AddMember(rootName, root, allocator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();

		rapidjson::PrettyWriter writer(buffer);
		outDocument.Accept(writer);
		return buffer.GetString();
	}

	//////////////////////////////////////////////////////////////////////////
	JsonObject& JsonObject::CreateObject(const String& key, ClassId typeHash)
	{
		RefPtr<JsonObject> pObject = MakeRef(New<JsonObject>(key, typeHash));
		pObject->m_Parent = this;
		m_Children.AppendBack(pObject);
		return *pObject;
	}

	//////////////////////////////////////////////////////////////////////////
	void JsonObject::Write(JsonSerializableType&& value)
	{
		m_Value = std::move(value);
	}

	//////////////////////////////////////////////////////////////////////////
	JsonObject& JsonObject::SerializeSubObject(const String& key, ClassId typeHash, const JsonSerializableType& value)
	{
		JsonObject& object = CreateObject(key, typeHash);
		object.m_Value = value;
		return object;
	}

	JsonObject& JsonObject::SerializeReflectedObject(const String& key, ClassId typeHash, const void* pObject)
	{
		JsonObject& object = CreateObject(key, typeHash);
		// todo...
		return object;
	}

	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		constexpr bool IsVectorType(ClassId hash)
		{
#define ISVEC(type) hash == classid(type)
			return ISVEC(float2)
				|| ISVEC(float3)
				|| ISVEC(float4)
				|| ISVEC(int2)
				|| ISVEC(int3)
				|| ISVEC(int4)
				|| ISVEC(uint2)
				|| ISVEC(uint3)
				|| ISVEC(uint4);
		}

		void ReadVectorType(JsonObject& readInto, const String& name, rapidjson::Value& value)
		{
			switch (readInto.GetTypeHash())
			{
			case ClassId("float2"):
				readInto.Write(float2(value["x"].GetFloat(), value["y"].GetFloat()));
				break;
			case ClassId("float3"):
				readInto.Write(float3(value["x"].GetFloat(), value["y"].GetFloat(), value["z"].GetFloat()));
				break;
			case ClassId("float4"):
				readInto.Write(float4(value["x"].GetFloat(), value["y"].GetFloat(), value["z"].GetFloat(), value["w"].GetFloat()));
				break;
			case ClassId("int2"):
				readInto.Write(int2(value["x"].GetInt(), value["y"].GetInt()));
				break;
			case ClassId("int3"):
				readInto.Write(int3(value["x"].GetInt(), value["y"].GetInt(), value["z"].GetInt()));
				break;
			case ClassId("int4"):
				readInto.Write(int4(value["x"].GetInt(), value["y"].GetInt(), value["z"].GetInt(), value["w"].GetInt()));
				break;
			case ClassId("uint2"):
				readInto.Write(uint2(value["x"].GetUint(), value["y"].GetUint()));
				break;
			case ClassId("uint3"):
				readInto.Write(uint3(value["x"].GetUint(), value["y"].GetUint(), value["z"].GetUint()));
				break;
			case ClassId("uint4"):
				readInto.Write(uint4(value["x"].GetUint(), value["y"].GetUint(), value["z"].GetUint(), value["w"].GetUint()));
				break;
			default:
				break;
			}
		}

		void JsonReadMemberRecursive(JsonObject& parent, const String& name, rapidjson::Value& value)
		{
			if (value.IsObject())
			{
				JsonObject& newObject = parent.CreateObject(name, ClassId::none);
				if (value.HasMember("TypeHash"))
				{
					newObject.SetTypeHash(value["TypeHash"].GetUint64());

					if (IsVectorType(newObject.GetTypeHash()))
					{
						ReadVectorType(newObject, name, value);
						return;
					}
				}

				for (auto& [subName, subValue] : value.GetObject())
				{
					if (subName == "TypeHash")
					{
						continue;
					}
					JsonReadMemberRecursive(newObject, subName.GetString(), subValue);
				}
			}
			else if (value.IsString())
			{
				parent.SerializeSubObject(name, ClassId("String"), value.GetString());
			}
			else if (value.IsFloat())
			{
				parent.SerializeSubObject(name, ClassId("float"), value.GetFloat());
			}
			else if (value.IsUint64())
			{
				parent.SerializeSubObject(name, ClassId("u64"), value.GetUint64());
			}
			else if (value.IsInt())
			{
				parent.SerializeSubObject(name, ClassId("i32"), value.GetInt());
			}
			else if (value.IsBool())
			{
				parent.SerializeSubObject(name, ClassId("bool"), value.GetBool());
			}			
		}
	}

	//////////////////////////////////////////////////////////////////////////
	JsonDeserializer::JsonDeserializer(const String& text)
	{
		rapidjson::Document document{};
		document.Parse(text.c_str());

		CHECK(document.IsObject());
		auto object = document.GetObject();

		for (auto& [name, value] : object)
		{
			CHECK(value.IsObject());
			CHECK(name.IsString());

			m_RootNode = MakeRef(New<JsonObject>(name.GetString(), ClassId::none));

			auto object = value.GetObject();
			for (auto& [subName, subValue] : object)
			{
				JsonReadMemberRecursive(*m_RootNode, subName.GetString(), subValue);
			}
		}
	}
}