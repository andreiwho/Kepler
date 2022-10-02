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
		void PushModule(TRef<TApplicationModule> pModule, EModulePushStrategy strategy = EModulePushStrategy::Normal);
		void RemoveModule(TRef<TApplicationModule> pModule);
		void Clear();
		void HandlePlatformEvent(const TPlatformEventBase& event);

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnRenderGUI();

		void Init();
		void Terminate();

	private:
		TDynArray<TRef<TApplicationModule>> m_Modules{};
	};
}