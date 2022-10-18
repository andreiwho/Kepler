#pragma once
#include "Core/Core.h"
#include "../EntityComponent.h"

namespace ke
{
	class DirectionalLightComponent : public EntityComponent
	{
	public:
		inline float3 GetColor() const { return m_Color; }
		inline float GetIntensity() const { return m_Intensity; }
		inline void SetColor(float3 color) { m_Color = color; }
		inline void SetIntensity(float intensity) { m_Intensity = intensity; }

	private:
		float3 m_Color{};
		float m_Intensity{};
	};
}