#pragma once
#include "Core/Types.h"
#include "Renderer/World/WorldTransform.h"

namespace Kepler
{
	class TTransformComponent
	{
	public:
		TTransformComponent(float3 Location = float3(), float3 Rotation = float3(), float3 Scale = float3(1.0f));

		TWorldTransform GetTransform() const { return Transform; }
		void SetTransform(TWorldTransform NewTransform);

		void SetLocation(float3 NewLocation);
		void SetRotation(float3 NewRotation);
		void SetScale(float3 NewScale);

		inline float3 GetLocation() const { return Transform.GetLocation(); }
		inline float3 GetRotation() const { return Transform.GetRotation(); }
		inline float3 GetScale() const { return Transform.GetScale(); }

	private:
		TWorldTransform Transform;
	};
}