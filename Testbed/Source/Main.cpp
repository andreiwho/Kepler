#include "Kepler.h"

using namespace ke;

DEFINE_UNIQUE_LOG_CHANNEL(LogTestbed, All);

class TTestbedModule : public EngineModule
{
public:
	virtual void OnAttach() override
	{
		KEPLER_INFO(LogTestbed, "Initialized Testbed module");
	}
};

class TTestbed : public ke::Engine
{
	using Base = Engine;

public:
	TTestbed(const TApplicationLaunchParams& LaunchParams)
		: Base(LaunchParams)
	{
	}

protected:
	virtual void ChildSetupModuleStack(TModuleStack& ModuleStack) override
	{
		ModuleStack.PushModule(MakeRef(New<TTestbedModule>()), EModulePushStrategy::Normal);
	}
};

TSharedPtr<Engine> ke::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	TSharedPtr<Engine> App = MakeShared<TTestbed>(LaunchParams);
	// ... Do some processing if needed
	return App;
}