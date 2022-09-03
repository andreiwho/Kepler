#pragma once
#include "Core/Types.h"
#include "WorldTransform.h"

namespace Kepler
{
	class TCamera
	{
	public:
		TCamera(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip, float3 InLocation = float3(), float3 InTarget = float3());

		matrix4x4 GenerateViewProjectionMatrix() const;

		inline float GetFOV() const { return FOV; }
		inline void SetFOV(float NewFOVDegrees) { FOV = glm::radians(NewFOVDegrees); }
		inline u32 GetFrustumWidth() const { return Width; }
		inline void SetFrustumWidth(u32 NewWidth) { Width = NewWidth; }
		inline u32 GetFrustumHeight() const { return Height; }
		inline void SetFrustumHeight(u32 NewHeight) { Height = NewHeight; }
		inline float GetNearClip() const { return NearClip; }
		inline void SetNearClip(float NewClip) { NearClip = NewClip; }
		inline float GetFarClip() const { return FarClip; }
		inline void SetFarClip(float NewClip) { FarClip = NewClip; }
		inline void SetLocation(float3 NewLocation) { Location = NewLocation; }
		inline float3 GetLocation() const { return Location; }
		inline void SetTarget(float3 NewTarget) { Target = NewTarget; }
		inline float3 GetTarget() const { return Target; }

	private:
		float FOV{};
		u32 Width{};
		u32 Height{};
		float NearClip{};
		float FarClip{};
		float3 Location{};
		float3 Target{};
	};
}