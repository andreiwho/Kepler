#include "WorldTransform.h"

namespace ke
{

	TWorldTransform::TWorldTransform(float3 location, float3 rotation, float3 scale)
		: m_Location(location)
		, m_Rotation(rotation)
		, m_Scale(scale)
	{
	}

	void TWorldTransform::SetLocation(float3 location)
	{
		m_Location = location;
	}

	void TWorldTransform::SetRotation(float3 rotation)
	{
		m_Rotation = rotation;
	}

	void TWorldTransform::SetScale(float3 scale)
	{
		m_Scale = scale;
	}

	matrix4x4 TWorldTransform::GenerateWorldMatrix() const
	{
		matrix4x4 worldMat = matrix4x4(1.0f);
		worldMat = glm::translate(worldMat, m_Location);
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.z), float3(0.0f, 0.0f, 1.0f));
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.y), float3(0.0f, 1.0f, 0.0f));
		worldMat = glm::rotate(worldMat, glm::radians(m_Rotation.x), float3(1.0f, 0.0f, 0.0f));
		worldMat = glm::scale(worldMat, m_Scale);
		return worldMat;
	}
}