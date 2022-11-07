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
		if (!Asset)
		{
			Asset = Await(AssetManager::Get()->FindAssetNode(Material->GetParentAssetPath()));
		}
	}

	void MaterialComponent::Set_Asset(AssetTreeNode* pAsset)
	{
		if (pAsset == Asset || !pAsset)
		{
			return;
		}

		if (auto pMaterial = TMaterialLoader::Get()->LoadMaterial(pAsset->GetPath()))
		{
			Material = pMaterial;
		}
		Asset = pAsset;
	}

}