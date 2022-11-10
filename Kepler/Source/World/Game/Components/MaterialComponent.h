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
		reflected_body();
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
		
		reflected kmeta(assettype = Material, set = Set_Asset)
		AssetTreeNode* Asset {nullptr};
		void Set_Asset(AssetTreeNode* pAsset);

	private:
		RefPtr<TMaterial> Material;
	};
}