#include "ModuleStack.h"

namespace ke
{
	void TModuleStack::PushModule(TRef<EngineModule> pModule, EModulePushStrategy strategy /*= EModulePushStrategy::Normal*/)
	{
		if (!pModule)
		{
			return;
		}

		switch (strategy)
		{
		case ke::EModulePushStrategy::Normal:
		{
			m_Modules.EmplaceBack(pModule);
		}
		break;
		case ke::EModulePushStrategy::Overlay:
		{
			Array<TRef<EngineModule>> newModules;
			newModules.Reserve(m_Modules.GetLength() + 1);
			newModules.EmplaceBack(pModule);
			for (TRef<EngineModule> pNewModule : m_Modules)
			{
				newModules.EmplaceBack(pNewModule);
			}
			m_Modules = newModules;
		}
		break;
		default:
			CRASH();
			break;
		}
		pModule->OnAttach();
	}

	void TModuleStack::RemoveModule(TRef<EngineModule> pModule)
	{
		if (!pModule)
		{
			return;
		}

		pModule->OnDetach();
		auto iter = m_Modules.FindIterator(
			[pModule](const TRef<EngineModule>& lhs)
			{
				return pModule.Raw() == lhs.Raw();
			});
		if (m_Modules.IsValidIterator(iter))
		{
			m_Modules.Remove(iter);
		}
		m_Modules.Shrink();
	}

	void TModuleStack::Clear()
	{
		for (auto pModule : m_Modules)
		{
			pModule->OnDetach();
		}

		m_Modules.Clear();
	}

	void TModuleStack::HandlePlatformEvent(const TPlatformEventBase& event)
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnPlatformEvent(event);
		}
	}

	void TModuleStack::OnUpdate(float deltaTime)
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnUpdate(deltaTime);
		}
	}

	void TModuleStack::OnRender()
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnRender();
		}
	}

	void TModuleStack::OnRenderGUI()
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnRenderGUI();
		}
	}

	void TModuleStack::Init()
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnInit();
		}
	}

	void TModuleStack::Terminate()
	{
		for (TRef<EngineModule> pModule : m_Modules)
		{
			pModule->OnTerminate();
		}
	}

}