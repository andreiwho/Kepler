#pragma once
#include "Renderer/World/Material.h"
#include "EntityComponent.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "MaterialComponent.gen.h"

namespace ke
{
	reflected kmeta(hideindetails)
	class MaterialComponent : public EntityComponent
	{
	public:
		MaterialComponent() = default;
		MaterialComponent(RefPtr<TMaterial> InMaterial);

		inline RefPtr<TMaterial> GetMaterial() { return Material; }
		void SetMaterial(RefPtr<TMaterial> InMaterial);

		inline const String& GetMaterialParentAssetPath() const
		{
			return Material->GetParentAssetPath();
		}

		inline bool UsesPrepass() const { return Material ? Material->UsesPrepass() : false; }

		String MaterialAssetPath;
		void OnMaterialPathChanged(const String& newPath);
		
		reflected kmeta(postchange = OnMaterialAssetChanged, assettype=Material)
		AssetTreeNode* Asset {nullptr};
		void OnMaterialAssetChanged(AssetTreeNode* newAsset);

	private:
		RefPtr<TMaterial> Material;
	};
}