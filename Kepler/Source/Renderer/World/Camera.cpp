#include "Camera.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	MathCamera::MathCamera(float fovd, float width, float height, float nearClip, float farClip, float3 location, float3 target)
		: FieldOfView(fovd)
		, Width(width >= 1.0f ? width : 1.0f)
		, Height(height >= 1.0f ? height : 1.0f)
		, NearClip(nearClip)
		, FarClip(farClip)
		, Location(location)
		, Target(target)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void MathCamera::SetTransform(const WorldTransform& transform)
	{
		const auto location = transform.GetLocation();
		const auto direction = ToEuler(transform.GetRotation());
		SetLocation(location);
		SetTarget(location + direction);
	}

	float3 MathCamera::ToEuler(float3 vec)
	{
		const float yaw = vec.z + 90.0f;
		const float pitch = vec.x;
		// const float Roll = Vector.y;

		float3 direction{};
		direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		direction.z = glm::sin(glm::radians(pitch));
		direction.y = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		return glm::normalize(direction);
	}

	//////////////////////////////////////////////////////////////////////////
	matrix4x4 MathCamera::GenerateViewProjectionMatrix() const
	{
		return GenerateProjectionMatrix() * GenerateViewMatrix();
	}

	matrix4x4 MathCamera::GenerateViewMatrix() const
	{
		return glm::lookAtRH(Location, Target, float3(0.0f, 0.0f, 1.0f));
	}

	matrix4x4 MathCamera::GenerateProjectionMatrix() const
	{
		return glm::perspectiveFovRH_ZO(glm::radians(FieldOfView), (float)Width, (float)Height, NearClip, FarClip);
	}

}