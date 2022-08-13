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

std::shared_ptr<TApplication> Kepler::MakeRuntimeApplication(TApplicationLaunchParams const& LaunchParams)
{
	std::shared_ptr<TApplication> App = std::make_shared<TTestbed>(LaunchParams);
	// ... Do some processing if needed
	return App;
}