#pragma once
#include "Core/Core.h"
#include "Platform/PlatformEvent.h"

namespace Kepler
{
	class TApplicationModule : public TRefCounted
	{
	public:
		virtual void OnAttach() {}
		virtual void OnInit() {}
		virtual void OnTerminate() {}
		virtual void OnDetach() {}
		virtual void OnPlatformEvent(const TPlatformEventBase& Event) {}
	};
}