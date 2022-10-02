#pragma once
#include "Core/Core.h"
#include "Platform/PlatformEvent.h"

namespace ke
{
	class TApplicationModule : public TRefCounted
	{
	public:
		virtual void OnAttach() {}
		virtual void OnInit() {}
		virtual void OnTerminate() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnRenderGUI() {}
		virtual void OnPlatformEvent(const TPlatformEventBase& event) {}
	};
}