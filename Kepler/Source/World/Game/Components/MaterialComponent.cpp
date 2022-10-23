#include "MaterialComponent.h"

namespace ke
{
	MaterialComponent::MaterialComponent(RefPtr<TMaterial> InMaterial)
		: Material(InMaterial)
	{
		MaterialAssetPath = Material->GetParentAssetPath();
	}

	void MaterialComponent::SetMaterial(RefPtr<TMaterial> InMaterial)
	{
		Material = InMaterial;
		MaterialAssetPath = Material->GetParentAssetPath();
	}

}