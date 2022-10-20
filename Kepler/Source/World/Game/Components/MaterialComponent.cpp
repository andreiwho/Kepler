#include "MaterialComponent.h"

namespace ke
{
	MaterialComponent::MaterialComponent(RefPtr<TMaterial> InMaterial)
		: Material(InMaterial)
	{

	}

	void MaterialComponent::SetMaterial(RefPtr<TMaterial> InMaterial)
	{
		Material = InMaterial;
	}

}