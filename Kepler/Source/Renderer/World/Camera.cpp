#include "Camera.h"

namespace Kepler
{
	TCamera::TCamera(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip, float3 InLocation, float3 InTarget)
		: FOV(glm::radians(InFOVDegrees))
		, Width(InWidth)
		, Height(InHeight)
		, NearClip(InNearClip)
		, FarClip(InFarClip)
		, Location(InLocation)
		, Target(InTarget)
	{
	}

	matrix4x4 TCamera::GenerateViewProjectionMatrix() const
	{
		matrix4x4 Projection = glm::perspectiveFovRH_ZO(FOV, (float)Width, (float)Height, NearClip, FarClip);
		matrix4x4 View = glm::lookAtRH(Location, Target, float3(0.0f, 0.0f, 1.0f));
		return Projection * View;
	}

}