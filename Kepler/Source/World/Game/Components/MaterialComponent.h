#pragma once
#include "Renderer/World/Material.h"

namespace Kepler
{
	class TMaterialComponent
	{
	public:
		TMaterialComponent() = default;
		TMaterialComponent(TRef<TGraphicsPipeline> InPipeline, TRef<TPipelineParamMapping> InParamMapping);

		inline TRef<TMaterial> GetMaterial() { return Material; }
		void SetMaterial(TRef<TMaterial> InMaterial);

	private:
		TRef<TMaterial> Material;
	};
}