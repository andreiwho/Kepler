#include "Kepler.h"
#include "Testbed.h"
#include "Reflection/ReflectionDatabase.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogTestbed, All);

	void TestbedModule::OnAttach()
	{
		KEPLER_INFO(LogTestbed, "Initialized Testbed module");
	}

	TestbedEngine::TestbedEngine(const TApplicationLaunchParams& LaunchParams) : Base(LaunchParams)
	{
	}

	void TestbedEngine::PushClass(RefPtr<ReflectedClass> cls)
	{
		ReflectionDatabase::Get()->PushClass(cls);
	}

	void TestbedEngine::ChildSetupModuleStack(TModuleStack& ModuleStack)
	{
		ModuleStack.PushModule(MakeRef(New<TestbedModule>()), EModulePushStrategy::Normal);
	}

	SharedPtr<Engine> ke::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
	{
		SharedPtr<TestbedEngine> App = MakeShared<TestbedEngine>(LaunchParams);
		App->FillReflectionDatabaseEntries();
		return App;
	}

}