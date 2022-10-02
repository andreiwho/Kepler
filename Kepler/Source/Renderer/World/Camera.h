#pragma once
#include "Core/Types.h"
#include "WorldTransform.h"

namespace ke
{
	class TCamera
	{
	public:
		TCamera(float InFOVDegrees, float InWidth, float InHeight, float InNearClip, float InFarClip, float3 InLocation = float3(), float3 InTarget = float3());

		matrix4x4 GenerateViewProjectionMatrix() const;
		matrix4x4 GenerateViewMatrix() const;
		matrix4x4 GenerateProjectionMatrix() const;

		inline float GetFOV() const { return FOV; }
		inline void SetFOV(float NewFOVDegrees) { FOV = NewFOVDegrees; }
		inline float GetFrustumWidth() const { return Width; }
		inline void SetFrustumWidth(float NewWidth) { Width = NewWidth; }
		inline float GetFrustumHeight() const { return Height; }
		inline void SetFrustumHeight(float NewHeight) { Height = NewHeight; }
		inline float GetNearClip() const { return NearClip; }
		inline void SetNearClip(float NewClip) { NearClip = NewClip; }
		inline float GetFarClip() const { return FarClip; }
		inline void SetFarClip(float NewClip) { FarClip = NewClip; }
		inline void SetLocation(float3 NewLocation) { Location = NewLocation; }
		inline float3 GetLocation() const { return Location; }
		inline void SetTarget(float3 NewTarget) { Target = NewTarget; }
		inline float3 GetTarget() const { return Target; }

		void SetTransform(const TWorldTransform& Transform);
		static float3 ToEuler(float3 Vector);

	private:
		float FOV{};
		float Width{};
		float Height{};
		float NearClip{};
		float FarClip{};
		float3 Location{};
		float3 Target{};
	};
}