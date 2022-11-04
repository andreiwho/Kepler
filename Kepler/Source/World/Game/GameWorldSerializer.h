#pragma once
#include "Async/Async.h"
#include "GameWorld.h"
#include "Core/Json/Serialization.h"

namespace ke
{
	struct SerializedFieldInfo
	{
		JsonSerializableType Data;
		ClassId TypeHash;
		Map<String, SerializedFieldInfo> SubFields;
	};

	struct SerializedObjectInfo
	{
		String Name;
		ClassId TypeHash;
		Map<String, SerializedFieldInfo> Fields;
	};

	//////////////////////////////////////////////////////////////////////////
	class GameWorldSerializer
	{
	public:
		GameWorldSerializer(RefPtr<GameWorld> pWorld);

		inline const String& GetWorldName() const& { return m_Name; }

		Map<String, Array<SerializedObjectInfo>> Serialize() const;
		TFuture<String> SerializeToJson() const;

	private:
		RefPtr<GameWorld> m_World;
		String m_Name;
	};

	class ReflectedObjectSerializer
	{
	public:
		template<typename T>
		static SerializedObjectInfo SerializeObject(T* pObject)
		{
			RefPtr<ReflectedClass> pClass = GetReflectedClass<T>();
			return SerializeObject(pClass->GetClassId());
		}

		static SerializedObjectInfo SerializeObject(ClassId typeHash, void* pObject);
	};

	//////////////////////////////////////////////////////////////////////////
	class GameWorldDeserializer
	{
	public:
		RefPtr<GameWorld> Deserialize(RefPtr<JsonObject> pJsonData);
		RefPtr<GameWorld> Deserialize(const String& worldName, Map<String, Array<SerializedObjectInfo>>& objects, u32 worldIndex = 0);
	};
}