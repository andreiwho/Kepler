#include "MaterialComponent.h"

namespace Kepler
{

	TMaterialComponent::TMaterialComponent(TRef<TGraphicsPipeline> InPipeline, TRef<TPipelineParamMapping> InParamMapping)
		:	Material(TMaterial::New(InPipeline, InParamMapping))
	{
	}

	void TMaterialComponent::SetMaterial(TRef<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}