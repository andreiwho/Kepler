#include "Camera.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	TCamera::TCamera(float InFOVDegrees, float InWidth, float InHeight, float InNearClip, float InFarClip, float3 InLocation, float3 InTarget)
		: FOV(InFOVDegrees)
		, Width(InWidth >= 1.0f ? InWidth : 1.0f)
		, Height(InHeight >= 1.0f ? InHeight : 1.0f)
		, NearClip(InNearClip)
		, FarClip(InFarClip)
		, Location(InLocation)
		, Target(InTarget)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void TCamera::SetTransform(const TWorldTransform& Transform)
	{
		const auto Location = Transform.GetLocation();
		const auto Direction = ToEuler(Transform.GetRotation());
		SetLocation(Location);
		SetTarget(Location + Direction);
	}

	float3 TCamera::ToEuler(float3 Vector)
	{
		const float Yaw = Vector.z + 90.0f;
		const float Pitch = Vector.x;
		// const float Roll = Vector.y;

		float3 Direction{};
		Direction.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
		Direction.z = glm::sin(glm::radians(Pitch));
		Direction.y = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
		return glm::normalize(Direction);
	}

	//////////////////////////////////////////////////////////////////////////
	matrix4x4 TCamera::GenerateViewProjectionMatrix() const
	{
		return GenerateProjectionMatrix() * GenerateViewMatrix();
	}

	matrix4x4 TCamera::GenerateViewMatrix() const
	{
		return glm::lookAtRH(Location, Target, float3(0.0f, 0.0f, 1.0f));
	}

	matrix4x4 TCamera::GenerateProjectionMatrix() const
	{
		return glm::perspectiveFovRH_ZO(glm::radians(FOV), (float)Width, (float)Height, NearClip, FarClip);
	}

}