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
				UUID classId = pClass->GetClassId();
				m_NativeClassIds.AppendBack(classId);
			}
		}

		void AddComponent(ClassId hash)
		{
			m_NativeClassIds.AppendBack(hash);
		}

		template<typename T>
		void RemoveComponent()
		{
			if (auto pClass = ReflectionDatabase::Get()->GetClass<T>())
			{
				UUID classId = pClass->GetClassId();
				auto iter = m_NativeClassIds.FindIterator([&](const UUID& id) { return id == classId; });
				m_NativeClassIds.Remove(iter);
				m_NativeClassIds.Shrink();
			}
		}

		const Array<UUID>& GetComponentIds() const& { return m_NativeClassIds; }

	private:
		Array<UUID> m_NativeClassIds;
	};
}