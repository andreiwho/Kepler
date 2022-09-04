#include "MaterialComponent.h"

namespace Kepler
{
	TMaterialComponent::TMaterialComponent(TRef<TGraphicsPipeline> InPipeline)
		: Material(TMaterial::New(InPipeline))
	{
	}

	TMaterialComponent::TMaterialComponent(TRef<TMaterial> InMaterial)
		: Material(InMaterial)
	{

	}

	void TMaterialComponent::SetMaterial(TRef<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}