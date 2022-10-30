#include "GameWorldSerializer.h"
#include "GameEntity.h"
#include "Helpers/EntityHelper.h"
#include "../Camera/CameraComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Core/Json/Serialization.h"
#include "Async/Async.h"
#include "../WorldRegistry.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"


namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogGameWorldSerializer, All);

	GameWorldSerializer::GameWorldSerializer(RefPtr<GameWorld> pWorld)
		: m_World(pWorld)
		, m_Name(pWorld->GetName())
	{
	}

	namespace
	{
		SerializedFieldInfo GetFieldInfoFor(ReflectedField& field, void* pObject)
		{
			SerializedFieldInfo outInfo;
			outInfo.TypeHash = field.GetTypeHash();

#define		GET_FIELD_VALUE(Type) case typehash64(#Type): outInfo.Data = *field.GetValueFor<Type>(pObject); break
			switch (field.GetTypeHash())
			{
				GET_FIELD_VALUE(String);
				GET_FIELD_VALUE(float);
				GET_FIELD_VALUE(float2);
				GET_FIELD_VALUE(float3);
				GET_FIELD_VALUE(float4);
				GET_FIELD_VALUE(bool);
				GET_FIELD_VALUE(i32);
				GET_FIELD_VALUE(int2);
				GET_FIELD_VALUE(int3);
				GET_FIELD_VALUE(int4);
				GET_FIELD_VALUE(u32);
				GET_FIELD_VALUE(uint2);
				GET_FIELD_VALUE(uint3);
				GET_FIELD_VALUE(uint4);
				GET_FIELD_VALUE(id64);
				GET_FIELD_VALUE(typehash64);
				GET_FIELD_VALUE(uuid64);
			case typehash64("AssetTreeNode"):
			{
				AssetTreeNode* pNode = field.GetValueFor<AssetTreeNode>(pObject);
				id64 id = pNode->GetUUID();
				outInfo.Data = id;
				outInfo.TypeHash = typehash64("id64");
			}
			break;
			default:
			{
				// Get subfields
				auto pClass = ReflectionDatabase::Get()->FindClassByTypeHash(field.GetTypeHash());
				if (pClass)
				{
					if (pClass->IsEnum())
					{
						outInfo.Data = *field.GetValueFor<i32>(pObject);
					}

					for (auto& [name, subField] : pClass->GetFields())
					{
						outInfo.SubFields[name] = GetFieldInfoFor(subField, field.GetValueFor<void*>(pObject));
					}
				}
			}
			break;
			}

			if (!outInfo.SubFields.IsEmpty())
			{
				outInfo.Data = std::monostate{};
			}
			return outInfo;
		}
	}

	Map<String, Array<SerializedComponentInfo>> GameWorldSerializer::Serialize() const
	{
		Map<String, Array<SerializedComponentInfo>> outComponentInfos;

		m_World->GetComponentView<NativeComponentContainer>().each(
			[&, this](entt::entity e, NativeComponentContainer& NCC)
			{
				String entityName = m_World->GetEntityName(e);
				auto& entity = m_World->GetEntityFromId(e);

				if (entity.ShouldHideInSceneGraph())
				{
					return;
				}

				for (const typehash64& type : NCC.GetComponentIds())
				{
					RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassByTypeHash(type);
					if (!pClass)
					{
						return;
					}

					SerializedComponentInfo info{};
					info.Name = pClass->GetName();
					info.TypeHash = type;

					auto pComponent = m_World->GetComponentById(type, e);
					for (auto& [name, field] : pClass->GetFields())
					{
						SerializedFieldInfo fieldInfo = GetFieldInfoFor(field, pComponent);
						info.Fields[name] = std::move(fieldInfo);
					}

					outComponentInfos[entityName].AppendBack(std::move(info));
				}
			});
		return outComponentInfos;
	}

	namespace
	{
		JsonObject& JsonSerializeFieldRecursive(JsonObject& parent, typehash64 typeHash, const String& name, SerializedFieldInfo& info)
		{
			JsonObject& fieldContainer = parent.SerializeSubObject(name, typeHash, info.Data);
			for (auto& [subName, subField] : info.SubFields)
			{
				JsonSerializeFieldRecursive(fieldContainer, subField.TypeHash, subName, subField);
			}
			return fieldContainer;
		}
	}

	TFuture<String> GameWorldSerializer::SerializeToJson() const
	{
		Map<String, Array<SerializedComponentInfo>> componentInfos = Serialize();
		JsonSerializer serializer{};
		JsonObject& root = serializer.CreateRootObject(GetWorldName());

		for (auto& [entityName, components] : componentInfos)
		{
			JsonObject& entity = root.CreateObject(entityName, typehash64::none);
			for (SerializedComponentInfo& component : components)
			{
				JsonObject& serializedComponent = entity.CreateObject(component.Name, component.TypeHash);
				for (auto& [name, field] : component.Fields)
				{
					JsonSerializeFieldRecursive(serializedComponent, field.TypeHash, name, field);
				}
			}
		}
		return Async([Serializer = std::move(serializer)] { return Serializer.ToString(); });
	}

	namespace
	{
		SerializedFieldInfo DeserializeFieldRecursive(RefPtr<JsonObject> pField)
		{
			SerializedFieldInfo outInfo{};
			outInfo.TypeHash = pField->GetTypeHash();

			bool bIsBaseType = true;
			switch (pField->GetTypeHash())
			{
			case typehash64("String"):
				outInfo.Data = pField->GetValueAs<String>();
				break;
			case typehash64("u64"):
				outInfo.Data = pField->GetValueAs<u64>();
				break;
			case typehash64("id64"):
				outInfo.Data = pField->GetValueAs<id64>();
				break;
			case typehash64("i32"):
				outInfo.Data = pField->GetValueAs<i32>();
				break;
			case typehash64("u32"):
				outInfo.Data = pField->GetValueAs<u32>();
				break;
			case typehash64("float"):
				outInfo.Data = pField->GetValueAs<float>();
				break;
			case typehash64("bool"):
				outInfo.Data = pField->GetValueAs<bool>();
				break;
			case typehash64("float2"):
				outInfo.Data = pField->GetValueAs<float2>();
				break;
			case typehash64("float3"):
				outInfo.Data = pField->GetValueAs<float3>();
				break;
			case typehash64("float4"):
				outInfo.Data = pField->GetValueAs<float4>();
				break;
			case typehash64("int2"):
				outInfo.Data = pField->GetValueAs<int2>();
				break;
			case typehash64("int3"):
				outInfo.Data = pField->GetValueAs<int3>();
				break;
			case typehash64("int4"):
				outInfo.Data = pField->GetValueAs<int4>();
				break;
			case typehash64("uint2"):
				outInfo.Data = pField->GetValueAs<uint2>();
				break;
			case typehash64("uint3"):
				outInfo.Data = pField->GetValueAs<uint3>();
				break;
			case typehash64("uint4"):
				outInfo.Data = pField->GetValueAs<uint4>();
				break;
			default:
				bIsBaseType = false;
				break;
			}

			if (!bIsBaseType)
			{
				for (RefPtr<JsonObject> pChild : pField->GetChildren())
				{
					outInfo.SubFields[pChild->GetKey()] = DeserializeFieldRecursive(pChild);
				}
			}

			return outInfo;
		}
	}

	namespace
	{
		void WriteDeserializedField(void* pHandler, ReflectedField& field, SerializedFieldInfo& fieldInfo)
		{
			bool bIsOfBaseType = true;

#define		SET_FIELD_VALUE(Type) case typehash64(#Type): field.SetValueFor(pHandler, &std::get<Type>(fieldInfo.Data)); break
			switch (fieldInfo.TypeHash)
			{
				SET_FIELD_VALUE(String);
				SET_FIELD_VALUE(float);
				SET_FIELD_VALUE(float2);
				SET_FIELD_VALUE(float3);
				SET_FIELD_VALUE(float4);
				SET_FIELD_VALUE(bool);
				SET_FIELD_VALUE(i32);
				SET_FIELD_VALUE(int2);
				SET_FIELD_VALUE(int3);
				SET_FIELD_VALUE(int4);
				SET_FIELD_VALUE(u32);
				SET_FIELD_VALUE(uint2);
				SET_FIELD_VALUE(uint3);
				SET_FIELD_VALUE(uint4);
				SET_FIELD_VALUE(id64);
				SET_FIELD_VALUE(typehash64);
				SET_FIELD_VALUE(uuid64);
			case typehash64("u64"):
			{
				switch (field.GetTypeHash())
				{
				case typehash64("u64"):
					field.SetValueFor(pHandler, &std::get<u64>(fieldInfo.Data));
					break;
				case typehash64("id64"):
				{
					id64 id = std::get<u64>(fieldInfo.Data);
					field.SetValueFor(pHandler, &id);
				}
				break;
				case typehash64("AssetTreeNode"):
				{
					id64 assetUUID = std::get<u64>(fieldInfo.Data);
					auto pNode = Await(AssetManager::Get()->FindAssetNode(assetUUID));
					if (pNode)
					{
						field.SetValueFor(pHandler, pNode.Raw());
					}
				}
				break;
				default:
					break;
				}
			}
			break;
			default:
				bIsOfBaseType = false;
				break;
			}

			if (!bIsOfBaseType)
			{
				auto pClass = ReflectionDatabase::Get()->FindClassByTypeHash(fieldInfo.TypeHash);
				if (pClass)
				{
					if (pClass->IsEnum())
					{
						field.SetValueFor(pHandler, &std::get<i32>(fieldInfo.Data));
					}
					else
					{
						void** ppHandledField = field.GetValueFor<void*>(pHandler);
						auto& subFields = pClass->GetFields();
						for (auto& [name, subField] : fieldInfo.SubFields)
						{
							WriteDeserializedField(ppHandledField, subFields[name], subField);
						}
					}
				}
			}
		}
	}

	RefPtr<GameWorld> GameWorldDeserializer::Deserialize(RefPtr<JsonObject> pJsonData)
	{
		CHECK(pJsonData->IsRoot());
		String name = pJsonData->GetKey();

		// Read all entities
		Map<String, Array<SerializedComponentInfo>> componentInfos;
		for (RefPtr<JsonObject> pChild : pJsonData->GetChildren())
		{
			Array<SerializedComponentInfo>& currentEntity = componentInfos[pChild->GetKey()];
			for (RefPtr<JsonObject> pComponent : pChild->GetChildren())
			{
				SerializedComponentInfo componentInfo{};
				componentInfo.Name = pComponent->GetKey();
				componentInfo.TypeHash = pComponent->GetTypeHash();

				for (RefPtr<JsonObject> pField : pComponent->GetChildren())
				{
					componentInfo.Fields[pField->GetKey()] = DeserializeFieldRecursive(pField);
				}

				currentEntity.AppendBack(componentInfo);
			}
		}
		return Deserialize(name, componentInfos);
	}

	RefPtr<GameWorld> GameWorldDeserializer::Deserialize(const String& worldName, Map<String, Array<SerializedComponentInfo>>& objects, u32 worldIndex)
	{
		auto pWorld = WorldRegistry::Get()->CreateWorldAtIndex<GameWorld>(worldIndex, worldName);
		for (auto& [name, components] : objects)
		{
			GameEntityId entity = pWorld->CreateEntityDeferred();
			for (auto& component : components)
			{
				EntityComponent* pNewComponent = pWorld->AddComponentByTypeHash(entity, component.TypeHash);
				CHECK(pNewComponent);

				RefPtr<ReflectedClass> pClass = ReflectionDatabase::Get()->FindClassByTypeHash(component.TypeHash);
				for (auto& [name, field] : component.Fields)
				{
					ReflectedField& reflectedField = pClass->GetFieldByName(name);
					WriteDeserializedField(pNewComponent, reflectedField, field);
				}
			}
			pWorld->FinishCreatingEntity(entity);
		}
		return pWorld;
	}
}