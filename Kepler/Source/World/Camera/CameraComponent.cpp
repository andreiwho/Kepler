#include "CameraComponent.h"

namespace ke
{

	CameraComponent::CameraComponent(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip)
		: m_Camera(InFOVDegrees, InWidth, InHeight, InNearClip, InFarClip)
	{
	}

	CameraComponent::~CameraComponent()
	{
	}

}