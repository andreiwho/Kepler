#include "Kepler.h"

namespace ke
{
	class TestbedModule : public EngineModule
	{
	public:
		virtual void OnAttach() override;
	};

	holding_reflection_data class TestbedEngine : public Engine
	{
		using Base = Engine;

	public:
		TestbedEngine(const TApplicationLaunchParams& LaunchParams);
		
		void FillReflectionDatabaseEntries();
		void PushClass(RefPtr<ReflectedClass> cls);

	protected:
		virtual void ChildSetupModuleStack(TModuleStack& ModuleStack) override;
	};


}