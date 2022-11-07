#pragma once
#include "Core/Core.h"
#include "../EntityComponent.h"
#include "DirectionalLightComponent.gen.h"

namespace ke
{
	reflected class DirectionalLightComponent : public EntityComponent
	{
		reflected_body();
	public:
		inline float3 GetColor() const { return Color; }
		inline float GetIntensity() const { return Intensity; }
		inline void SetColor(float3 color) { Color = color; }
		inline void SetIntensity(float intensity) { Intensity = intensity; }

		reflected kmeta(editspeed=0.01f) 
		float3 Color{};
		
		reflected kmeta(editspeed=0.01f) 
		float Intensity{};
	};
}