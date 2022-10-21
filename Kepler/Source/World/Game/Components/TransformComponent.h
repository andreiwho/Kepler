#pragma once
#include "Core/Types.h"
#include "Renderer/World/WorldTransform.h"
#include "EntityComponent.h"
#include "TransformComponent.gen.h"

namespace ke
{
	reflected class TTransformComponent : public EntityComponent
	{
	public:
		TTransformComponent(float3 Location = float3(), float3 Rotation = float3(), float3 Scale = float3(1.0f));

		WorldTransform GetTransform() const { return Transform; }
		void SetTransform(WorldTransform NewTransform);

		void SetLocation(float3 NewLocation);
		void SetRotation(float3 NewRotation);
		void SetScale(float3 NewScale);

		inline float3 GetLocation() const { return Transform.GetLocation(); }
		inline float3 GetRotation() const { return Transform.GetRotation(); }
		inline float3 GetScale() const { return Transform.GetScale(); }

		float3 GetForwardVector() const;
		float3 GetRightVector() const;
		float3 GetUpVector() const;

	private:
		WorldTransform Transform;
	};
}