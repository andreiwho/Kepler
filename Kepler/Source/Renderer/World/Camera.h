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

		inline float GetFOV() const { return m_FieldOfView; }
		inline void SetFOV(float fov) { m_FieldOfView = fov; }
		inline float GetFrustumWidth() const { return m_Width; }
		inline void SetFrustumWidth(float width) { m_Width = width; }
		inline float GetFrustumHeight() const { return m_Height; }
		inline void SetFrustumHeight(float height) { m_Height = height; }
		inline float GetNearClip() const { return m_NearClip; }
		inline void SetNearClip(float clip) { m_NearClip = clip; }
		inline float GetFarClip() const { return m_FarClip; }
		inline void SetFarClip(float clip) { m_FarClip = clip; }
		inline void SetLocation(float3 location) { m_Location = location; }
		inline float3 GetLocation() const { return m_Location; }
		inline void SetTarget(float3 target) { m_Target = target; }
		inline float3 GetTarget() const { return m_Target; }

		void SetTransform(const WorldTransform& Transform);
		static float3 ToEuler(float3 Vector);

	public:
		reflected float m_FieldOfView{};
		reflected float m_Width{};
		reflected float m_Height{};
		reflected float m_NearClip{};
		reflected float m_FarClip{};
		reflected float3 m_Location{};
		reflected float3 m_Target{};
	};
}