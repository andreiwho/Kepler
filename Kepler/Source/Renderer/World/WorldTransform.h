#pragma once
#include "Core/Core.h"
#include "WorldTransform.gen.h"

namespace ke
{
	reflected class WorldTransform
	{
		reflected_body();
	public:
		WorldTransform() = default;
		WorldTransform(float3 location, float3 rotation = float3(), float3 scale = float3(1.0f));

		inline float3 GetLocation() const { return Location; }
		inline float3 GetRotation() const { return Rotation; }
		inline float3 GetScale() const { return Scale; }
		quaternion RotationToQuat() const;

		void SetLocation(float3 location);
		void SetRotation(float3 rotation);
		void SetScale(float3 scale);

		matrix4x4 GenerateWorldMatrix() const;
		matrix3x3 GenerateNormalMatrix() const;
		float3 RotationToEulerNormalized() const;
		float3 RotationToEuler() const;

	private:
		reflected float3 Location = float3();
		reflected float3 Rotation = float3();
		reflected float3 Scale = float3(1.0f);
	};
}