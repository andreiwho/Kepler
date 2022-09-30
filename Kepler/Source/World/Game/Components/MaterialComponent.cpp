#include "MaterialComponent.h"

namespace Kepler
{
	TMaterialComponent::TMaterialComponent(TRef<TMaterial> InMaterial)
		: Material(InMaterial)
	{

	}

	void TMaterialComponent::SetMaterial(TRef<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}