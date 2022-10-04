#include "Camera.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	MathCamera::MathCamera(float fovd, float width, float height, float nearClip, float farClip, float3 location, float3 target)
		: m_FieldOfView(fovd)
		, m_Width(width >= 1.0f ? width : 1.0f)
		, m_Height(height >= 1.0f ? height : 1.0f)
		, m_NearClip(nearClip)
		, m_FarClip(farClip)
		, m_Location(location)
		, m_Target(target)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void MathCamera::SetTransform(const TWorldTransform& transform)
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
		return glm::lookAtRH(m_Location, m_Target, float3(0.0f, 0.0f, 1.0f));
	}

	matrix4x4 MathCamera::GenerateProjectionMatrix() const
	{
		return glm::perspectiveFovRH_ZO(glm::radians(m_FieldOfView), (float)m_Width, (float)m_Height, m_NearClip, m_FarClip);
	}

}