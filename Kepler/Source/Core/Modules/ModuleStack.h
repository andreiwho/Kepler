#pragma once
#include "Core/Core.h"
#include "Module.h"

namespace ke
{
	enum class EModulePushStrategy
	{
		Normal,
		Overlay,
	};

	class TModuleStack
	{
	public:
		void PushModule(TRef<EngineModule> pModule, EModulePushStrategy strategy = EModulePushStrategy::Normal);
		void RemoveModule(TRef<EngineModule> pModule);
		void Clear();
		void HandlePlatformEvent(const TPlatformEventBase& event);

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnRenderGUI();

		void Init();
		void Terminate();

	private:
		Array<TRef<EngineModule>> m_Modules{};
	};
}