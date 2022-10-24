#pragma once
#include "Core/Types.h"
#include "WorldTransform.h"
#include "Camera.gen.h"

namespace ke
{
	reflected class MathCamera
	{
	public:
		MathCamera(float fovd, float width, float height, float nearClip, float farClip, float3 location = float3(), float3 target = float3());

		matrix4x4 GenerateViewProjectionMatrix() const;
		matrix4x4 GenerateViewMatrix() const;
		matrix4x4 GenerateProjectionMatrix() const;

		inline float GetFOV() const { return FieldOfView; }
		inline void SetFOV(float fov) { FieldOfView = fov; }
		inline float GetFrustumWidth() const { return Width; }
		inline void SetFrustumWidth(float width) { Width = width; }
		inline float GetFrustumHeight() const { return Height; }
		inline void SetFrustumHeight(float height) { Height = height; }
		inline float GetNearClip() const { return NearClip; }
		inline void SetNearClip(float clip) { NearClip = clip; }
		inline float GetFarClip() const { return FarClip; }
		inline void SetFarClip(float clip) { FarClip = clip; }
		inline void SetLocation(float3 location) { Location = location; }
		inline float3 GetLocation() const { return Location; }
		inline void SetTarget(float3 target) { Target = target; }
		inline float3 GetTarget() const { return Target; }

		void SetTransform(const WorldTransform& Transform);
		static float3 ToEuler(float3 Vector);

	public:
		reflected float FieldOfView{};
		reflected float Width{};
		reflected float Height{};
		reflected float NearClip{};
		reflected float FarClip{};
		reflected float3 Location{};
		reflected float3 Target{};
	};
}