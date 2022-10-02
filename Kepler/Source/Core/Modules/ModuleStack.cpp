#include "ModuleStack.h"

namespace ke
{
	void TModuleStack::PushModule(TRef<TApplicationModule> Module, EModulePushStrategy Strategy /*= EModulePushStrategy::Normal*/)
	{
		if (!Module)
		{
			return;
		}

		switch (Strategy)
		{
		case ke::EModulePushStrategy::Normal:
		{
			Modules.EmplaceBack(Module);
		}
		break;
		case ke::EModulePushStrategy::Overlay:
		{
			TDynArray<TRef<TApplicationModule>> NewModules;
			NewModules.Reserve(Modules.GetLength() + 1);
			NewModules.EmplaceBack(Module);
			for (TRef<TApplicationModule> Module : Modules)
			{
				NewModules.EmplaceBack(Module);
			}
			Modules = NewModules;
		}
		break;
		default:
			CRASH();
			break;
		}
		Module->OnAttach();
	}

	void TModuleStack::RemoveModule(TRef<TApplicationModule> Module)
	{
		if (!Module)
		{
			return;
		}

		Module->OnDetach();
		auto Iterator = Modules.FindIterator(
			[Module](const TRef<TApplicationModule>& Lhs)
			{
				return Module.Raw() == Lhs.Raw();
			});
		if (Modules.IsValidIterator(Iterator))
		{
			Modules.Remove(Iterator);
		}
		Modules.Shrink();
	}

	void TModuleStack::Clear()
	{
		for (auto Module : Modules)
		{
			Module->OnDetach();
		}

		Modules.Clear();
	}

	void TModuleStack::HandlePlatformEvent(const TPlatformEventBase& Event)
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnPlatformEvent(Event);
		}
	}

	void TModuleStack::OnUpdate(float DeltaTime)
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnUpdate(DeltaTime);
		}
	}

	void TModuleStack::OnRender()
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnRender();
		}
	}

	void TModuleStack::OnRenderGUI()
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnRenderGUI();
		}
	}

	void TModuleStack::Init()
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnInit();
		}
	}

	void TModuleStack::Terminate()
	{
		for (TRef<TApplicationModule> Module : Modules)
		{
			Module->OnTerminate();
		}
	}

}