#include "WorldTransform.h"

namespace ke
{

	TWorldTransform::TWorldTransform(float3 InLocation, float3 InRotation, float3 InScale)
		: Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
	{
	}

	void TWorldTransform::SetLocation(float3 InLocation)
	{
		Location = InLocation;
	}

	void TWorldTransform::SetRotation(float3 InRotation)
	{
		Rotation = InRotation;
	}

	void TWorldTransform::SetScale(float3 InScale)
	{
		Scale = InScale;
	}

	matrix4x4 TWorldTransform::GenerateWorldMatrix() const
	{
		matrix4x4 World = matrix4x4(1.0f);
		World = glm::translate(World, Location);
		World = glm::rotate(World, glm::radians(Rotation.z), float3(0.0f, 0.0f, 1.0f));
		World = glm::rotate(World, glm::radians(Rotation.y), float3(0.0f, 1.0f, 0.0f));
		World = glm::rotate(World, glm::radians(Rotation.x), float3(1.0f, 0.0f, 0.0f));
		World = glm::scale(World, Scale);
		return World;
	}
}