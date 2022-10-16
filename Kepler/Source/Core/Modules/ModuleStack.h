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
		void PushModule(RefPtr<EngineModule> pModule, EModulePushStrategy strategy = EModulePushStrategy::Normal);
		void RemoveModule(RefPtr<EngineModule> pModule);
		void Clear();
		void HandlePlatformEvent(const TPlatformEventBase& event);

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnRenderGUI();
		void OnPostWorldInit();

		void Init();
		void Terminate();

	private:
		Array<RefPtr<EngineModule>> m_Modules{};
	};
}