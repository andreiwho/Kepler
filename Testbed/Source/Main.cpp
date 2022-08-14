#include "Kepler.h"

using namespace Kepler;

class TTestbed : public Kepler::TApplication
{
	using Base = TApplication;

public:
	TTestbed(const TApplicationLaunchParams& LaunchParams)
		: Base(LaunchParams)
	{}
};

TRef<TApplication> Kepler::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	TRef<TApplication> App = MakeRef<TTestbed>(LaunchParams);
	// ... Do some processing if needed
	return App;
}