#include "CameraComponent.h"

namespace Kepler
{

	TCameraComponent::TCameraComponent(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip)
		: Camera(InFOVDegrees, InWidth, InHeight, InNearClip, InFarClip)
	{
	}

	TCameraComponent::~TCameraComponent()
	{
	}

}