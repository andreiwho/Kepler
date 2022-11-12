#include "TransformComponent.h"
#include "Renderer/World/Camera.h"

namespace ke
{

	TransformComponent::TransformComponent(float3 Location, float3 Rotation, float3 Scale)
		:Transform(Location, Rotation, Scale)
	{
	}

	void TransformComponent::SetTransform(WorldTransform NewTransform)
	{
		Transform = NewTransform;
	}

	void TransformComponent::SetLocation(float3 NewLocation)
	{
		Transform.SetLocation(NewLocation);
	}

	void TransformComponent::SetRotation(float3 NewRotation)
	{
		Transform.SetRotation(NewRotation);
	}

	void TransformComponent::SetScale(float3 NewScale)
	{
		Transform.SetScale(NewScale);
	}

	float3 TransformComponent::GetForwardVector() const
	{
		return Transform.RotationToEulerNormalized();
	}

	float3 TransformComponent::GetRightVector() const
	{
		float3 WorldUp = float3(0.0f, 0.0f, 1.0f);
		return glm::normalize(glm::cross(GetForwardVector(), WorldUp));
	}

	float3 TransformComponent::GetUpVector() const
	{
		return glm::normalize(glm::cross(GetRightVector(), GetForwardVector()));
	}

}