#pragma once
#include "World/Game/Components/EntityComponent.h"
#include "NativeScriptComponent.h"
#include "Reflection/ReflectionDatabase.h"

namespace ke
{
	class NativeComponentContainer : public EntityComponent
	{
	public:
		template<typename T>
		void AddComponent()
		{
			if (auto pClass = ReflectionDatabase::Get()->GetClass<T>())
			{
				id64 classId = pClass->GetClassId();
				m_NativeClassIds.AppendBack(classId);
			}
		}

		template<typename T>
		void RemoveComponent()
		{
			if (auto pClass = ReflectionDatabase::Get()->GetClass<T>())
			{
				id64 classId = pClass->GetClassId();
				auto iter = m_NativeClassIds.FindIterator([&](const id64& id) { return id == classId; });
				m_NativeClassIds.Remove(iter);
				m_NativeClassIds.Shrink();
			}
		}

		const Array<id64>& GetComponentIds() const& { return m_NativeClassIds; }

	private:
		Array<id64> m_NativeClassIds;
	};
}