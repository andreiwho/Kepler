#include "Kepler.h"

using namespace ke;

DEFINE_UNIQUE_LOG_CHANNEL(LogTestbed, All);

class TestbedModule : public EngineModule
{
public:
	virtual void OnAttach() override
	{
		KEPLER_INFO(LogTestbed, "Initialized Testbed module");
	}
};

class TestbedEngine : public ke::Engine
{
	using Base = Engine;

public:
	TestbedEngine(const TApplicationLaunchParams& LaunchParams)
		: Base(LaunchParams)
	{
	}

protected:
	virtual void ChildSetupModuleStack(TModuleStack& ModuleStack) override
	{
		ModuleStack.PushModule(MakeRef(New<TestbedModule>()), EModulePushStrategy::Normal);
	}
};

SharedPtr<Engine> ke::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	SharedPtr<Engine> App = MakeShared<TestbedEngine>(LaunchParams);
	// ... Do some processing if needed
	return App;
}