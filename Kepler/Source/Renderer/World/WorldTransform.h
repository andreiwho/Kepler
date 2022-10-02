#pragma once
#include "Core/Core.h"

namespace ke
{
	class TWorldTransform
	{
	public:
		TWorldTransform() = default;
		TWorldTransform(float3 InLocation, float3 InRotation = float3(), float3 InScale = float3(1.0f));

		inline float3 GetLocation() const { return Location; }
		inline float3 GetRotation() const { return Rotation; }
		inline float3 GetScale() const { return Scale; }

		void SetLocation(float3 InLocation);
		void SetRotation(float3 InRotation);
		void SetScale(float3 InScale);

		matrix4x4 GenerateWorldMatrix() const;

	private:
		float3 Location = float3();
		float3 Rotation = float3();
		float3 Scale = float3(1.0f);
	};
}