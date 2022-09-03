#include "MaterialComponent.h"

namespace Kepler
{
	TMaterialComponent::TMaterialComponent(TRef<TGraphicsPipeline> InPipeline)
		: Material(TMaterial::New(InPipeline))
	{
	}

	void TMaterialComponent::SetMaterial(TRef<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}