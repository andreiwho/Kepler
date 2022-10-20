#include "WorldTransform.h"

namespace ke
{

	WorldTransform::WorldTransform(float3 location, float3 rotation, float3 scale)
		: m_Location(location)
		, m_Rotation(rotation)
		, m_Scale(scale)
	{
	}

	void WorldTransform::SetLocation(float3 location)
	{
		m_Location = location;
	}

	void WorldTransform::SetRotation(float3 rotation)
	{
		m_Rotation = rotation;
	}

	void WorldTransform::SetScale(float3 scale)
	{
		m_Scale = scale;
	}

	matrix4x4 WorldTransform::GenerateWorldMatrix() const
	{
		matrix4x4 worldMat = matrix4x4(1.0f);
		worldMat = glm::translate(worldMat, m_Location);
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.z), float3(0.0f, 0.0f, 1.0f));
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.y), float3(0.0f, 1.0f, 0.0f));
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.x), float3(1.0f, 0.0f, 0.0f));
		worldMat = glm::scale(worldMat, m_Scale);
		return worldMat;
	}

	matrix3x3 WorldTransform::GenerateNormalMatrix() const
	{
		const auto worldMat = GenerateWorldMatrix();
		return glm::transpose(glm::inverse(worldMat));
	}

	float3 WorldTransform::RotationToEuler() const
	{
		const float yaw = m_Rotation.z + 90.0f;
		const float pitch = m_Rotation.x;
		// const float Roll = Vector.y;

		float3 direction{};
		direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		direction.z = glm::sin(glm::radians(pitch));
		direction.y = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		return glm::normalize(direction);
	}

}