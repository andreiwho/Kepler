#include "MaterialComponent.h"
#include "Tools/MaterialLoader.h"

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

	void MaterialComponent::OnMaterialPathChanged(const String& newPath)
	{
		if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(newPath))
		{
			Material = pMaterial;
		}
	}

}