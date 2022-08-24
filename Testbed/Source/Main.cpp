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

TSharedPtr<TApplication> Kepler::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	TSharedPtr<TApplication> App = MakeShared<TTestbed>(LaunchParams);
	// ... Do some processing if needed
	return App;
}