#pragma once
#include "World/Game/Components/EntityComponent.h"
#include "NativeScriptComponent.h"

namespace ke
{
	class NativeScriptContainerComponent : public EntityComponent
	{
	public:
		NativeScriptContainerComponent();

		void Init();
		void Update(float deltaTime);
		void Destroying();

		template<typename T>
		void AddComponent()
		{
		}

		template<typename T>
		void RemoveComponent()
		{
		}
	};
}