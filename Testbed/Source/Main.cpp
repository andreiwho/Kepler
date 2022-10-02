#include "Kepler.h"

using namespace ke;

DEFINE_UNIQUE_LOG_CHANNEL(LogTestbed);

class TTestbedModule : public TApplicationModule
{
public:
	virtual void OnAttach() override
	{
		KEPLER_INFO(LogTestbed, "Initialized Testbed module");
	}
};

class TTestbed : public ke::TApplication
{
	using Base = TApplication;

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

TSharedPtr<TApplication> ke::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	TSharedPtr<TApplication> App = MakeShared<TTestbed>(LaunchParams);
	// ... Do some processing if needed
	return App;
}