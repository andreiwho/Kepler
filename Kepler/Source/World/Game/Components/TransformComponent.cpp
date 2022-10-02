#include "TransformComponent.h"
#include "Renderer/World/Camera.h"

namespace ke
{

	TTransformComponent::TTransformComponent(float3 Location, float3 Rotation, float3 Scale)
		:Transform(Location, Rotation, Scale)
	{
	}

	void TTransformComponent::SetTransform(TWorldTransform NewTransform)
	{
		Transform = NewTransform;
	}

	void TTransformComponent::SetLocation(float3 NewLocation)
	{
		Transform.SetLocation(NewLocation);
	}

	void TTransformComponent::SetRotation(float3 NewRotation)
	{
		Transform.SetRotation(NewRotation);
	}

	void TTransformComponent::SetScale(float3 NewScale)
	{
		Transform.SetScale(NewScale);
	}

	float3 TTransformComponent::GetForwardVector() const
	{
		return TCamera::ToEuler(Transform.GetRotation());
	}

	float3 TTransformComponent::GetRightVector() const
	{
		float3 WorldUp = float3(0.0f, 0.0f, 1.0f);
		return glm::normalize(glm::cross(GetForwardVector(), WorldUp));
	}

	float3 TTransformComponent::GetUpVector() const
	{
		return glm::normalize(glm::cross(GetRightVector(), GetForwardVector()));
	}

}