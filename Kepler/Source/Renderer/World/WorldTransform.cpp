#include "WorldTransform.h"

namespace ke
{

	WorldTransform::WorldTransform(float3 location, float3 rotation, float3 scale)
		: Location(location)
		, Rotation(rotation)
		, Scale(scale)
	{
	}

	quaternion WorldTransform::RotationToQuat() const
	{
		quaternion quat = glm::identity<quaternion>();
		quat = glm::rotate(quat, glm::radians(Rotation.z + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		quat = glm::rotate(quat, glm::radians(-Rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
		quat = glm::rotate(quat, glm::radians(Rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
		return quat;
	}

	void WorldTransform::SetLocation(float3 location)
	{
		Location = location;
	}

	void WorldTransform::SetRotation(float3 rotation)
	{
		Rotation = rotation;
	}

	void WorldTransform::SetScale(float3 scale)
	{
		Scale = scale;
	}

	matrix4x4 WorldTransform::GenerateWorldMatrix() const
	{
		matrix4x4 worldMat = matrix4x4(1.0f);
		const quaternion quat = RotationToQuat();

		worldMat = glm::translate(worldMat, Location);
		worldMat *= glm::toMat4(quat);
		worldMat = glm::scale(worldMat, Scale);

		return worldMat;
	}

	matrix3x3 WorldTransform::GenerateNormalMatrix() const
	{
		const auto worldMat = GenerateWorldMatrix();
		return glm::transpose(glm::inverse(worldMat));
	}

	float3 WorldTransform::RotationToEulerNormalized() const
	{
		return glm::normalize(RotationToEuler());
	}

	float3 WorldTransform::RotationToEuler() const
	{
		return glm::rotate(RotationToQuat(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

}