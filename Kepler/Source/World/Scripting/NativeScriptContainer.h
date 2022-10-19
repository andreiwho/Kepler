#pragma once
#include "World/Game/Components/EntityComponent.h"

namespace ke
{
	class NativeScriptContainerComponent : public EntityComponent
	{
	public:
		NativeScriptContainerComponent();
		
		void Init();
		void Update(float deltaTime);
		void Destroying();
	
	private:
	};
}