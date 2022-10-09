#pragma once
#include "Core/Core.h"

namespace ke
{
	class AmbientLightComponent
	{
	public:
		inline float3 GetColor() const { return m_Color; }
		inline void SetColor(float3 newColor) { m_Color = newColor; }

	private:
		float3 m_Color{};
	};
}