#include "ModuleStack.h"

namespace ke
{
	void TModuleStack::PushModule(RefPtr<EngineModule> pModule, EModulePushStrategy strategy /*= EModulePushStrategy::Normal*/)
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
			Array<RefPtr<EngineModule>> newModules;
			newModules.Reserve(m_Modules.GetLength() + 1);
			newModules.EmplaceBack(pModule);
			for (RefPtr<EngineModule> pNewModule : m_Modules)
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

	void TModuleStack::RemoveModule(RefPtr<EngineModule> pModule)
	{
		if (!pModule)
		{
			return;
		}

		pModule->OnDetach();
		auto iter = m_Modules.FindIterator(
			[pModule](const RefPtr<EngineModule>& lhs)
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
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnPlatformEvent(event);
		}
	}

	void TModuleStack::OnUpdate(float deltaTime)
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnUpdate(deltaTime);
		}
	}

	void TModuleStack::OnRender()
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnRender();
		}
	}

	void TModuleStack::OnRenderGUI()
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnRenderGUI();
		}
	}

	void TModuleStack::OnPostWorldInit()
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->PostWorldInit();
		}
	}

	void TModuleStack::Init()
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnInit();
		}
	}

	void TModuleStack::Terminate()
	{
		for (RefPtr<EngineModule> pModule : m_Modules)
		{
			pModule->OnTerminate();
		}
	}

}