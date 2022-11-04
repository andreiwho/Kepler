#include "MaterialComponent.h"
#include "Tools/MaterialLoader.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"

namespace ke
{
	MaterialComponent::MaterialComponent(RefPtr<TMaterial> InMaterial)
		: Material(InMaterial)
	{
		SetMaterial(InMaterial);
	}

	void MaterialComponent::SetMaterial(RefPtr<TMaterial> InMaterial)
	{
		Material = InMaterial;
		MaterialAssetPath = Material->GetParentAssetPath();

		if (!Asset)
		{
			Asset = Await(AssetManager::Get()->FindAssetNode(MaterialAssetPath));
		}
	}

	void MaterialComponent::OnMaterialPathChanged(const String& newPath)
	{
		if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(MaterialAssetPath))
		{
			Material = pMaterial;
		}
	}

	void MaterialComponent::OnMaterialAssetChanged(AssetTreeNode* newAsset)
	{
		if (newAsset)
		{
			MaterialAssetPath = newAsset->GetPath();
			OnMaterialPathChanged(MaterialAssetPath);
		}

	}

}