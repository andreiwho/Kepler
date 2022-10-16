#include "MaterialComponent.h"

namespace ke
{
	TMaterialComponent::TMaterialComponent(RefPtr<TMaterial> InMaterial)
		: Material(InMaterial)
	{

	}

	void TMaterialComponent::SetMaterial(RefPtr<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}