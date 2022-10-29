#pragma once
#include "Async/Async.h"
#include "GameWorld.h"
#include "Core/Json/Serialization.h"

namespace ke
{
	struct SerializedFieldInfo
	{
		JsonSerializableType Data;
		typehash64 TypeHash;
		Map<String, SerializedFieldInfo> SubFields;
	};

	struct SerializedComponentInfo
	{
		String Name;
		typehash64 TypeHash;
		Map<String, SerializedFieldInfo> Fields;
	};

	//////////////////////////////////////////////////////////////////////////
	class GameWorldSerializer
	{
	public:
		GameWorldSerializer(RefPtr<GameWorld> pWorld);

		inline const String& GetWorldName() const& { return m_Name; }

		Map<String, Array<SerializedComponentInfo>> Serialize() const;
		TFuture<String> SerializeToJson() const;

	private:
		RefPtr<GameWorld> m_World;
		String m_Name;
	};

	//////////////////////////////////////////////////////////////////////////
	class GameWorldDeserializer
	{
	public:
		RefPtr<GameWorld> Deserialize(RefPtr<JsonObject> pJsonData);
		RefPtr<GameWorld> Deserialize(const String& worldName, Map<String, Array<SerializedComponentInfo>>& objects);
	};
}