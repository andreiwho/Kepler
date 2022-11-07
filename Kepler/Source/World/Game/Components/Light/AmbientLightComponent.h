#pragma once
#include "Core/Core.h"
#include "../EntityComponent.h"
#include "AmbientLightComponent.gen.h"

namespace ke
{
	reflected class AmbientLightComponent : public EntityComponent
	{
		reflected_body();
	public:
		inline float3 GetColor() const { return Color; }
		inline void SetColor(float3 newColor) { Color = newColor; }

		reflected kmeta(editspeed=0.01f)
		float3 Color{};
	};
}