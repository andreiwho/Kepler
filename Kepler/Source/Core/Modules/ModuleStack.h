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
		void PushModule(TRef<TApplicationModule> Module, EModulePushStrategy Strategy = EModulePushStrategy::Normal);
		void RemoveModule(TRef<TApplicationModule> Module);
		void Clear();
		void HandlePlatformEvent(const TPlatformEventBase& Event);

		void OnUpdate(float DeltaTime);
		void OnRender();
		void OnRenderGUI();

		void Init();
		void Terminate();

	private:
		TDynArray<TRef<TApplicationModule>> Modules{};
	};
}