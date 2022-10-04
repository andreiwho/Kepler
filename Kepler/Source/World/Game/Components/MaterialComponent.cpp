#include "MaterialComponent.h"

namespace ke
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