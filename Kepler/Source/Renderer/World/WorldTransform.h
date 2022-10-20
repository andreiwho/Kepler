#pragma once
#include "Core/Core.h"

namespace ke
{
	class WorldTransform
	{
	public:
		WorldTransform() = default;
		WorldTransform(float3 location, float3 rotation = float3(), float3 scale = float3(1.0f));

		inline float3 GetLocation() const { return m_Location; }
		inline float3 GetRotation() const { return m_Rotation; }
		inline float3 GetScale() const { return m_Scale; }

		void SetLocation(float3 location);
		void SetRotation(float3 rotation);
		void SetScale(float3 scale);

		matrix4x4 GenerateWorldMatrix() const;
		matrix3x3 GenerateNormalMatrix() const;
		float3 RotationToEuler() const;

	private:
		float3 m_Location = float3();
		float3 m_Rotation = float3();
		float3 m_Scale = float3(1.0f);
	};
}