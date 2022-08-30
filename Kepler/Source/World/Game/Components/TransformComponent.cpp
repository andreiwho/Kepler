#include "TransformComponent.h"

namespace Kepler
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
}