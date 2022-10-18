#pragma once
#include "Core/Core.h"
#include "../EntityComponent.h"

namespace ke
{
	class AmbientLightComponent : public EntityComponent
	{
	public:
		inline float3 GetColor() const { return m_Color; }
		inline void SetColor(float3 newColor) { m_Color = newColor; }

	private:
		float3 m_Color{};
	};
}