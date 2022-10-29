#pragma once
#include "Renderer/World/Material.h"
#include "EntityComponent.h"
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

		inline bool UsesPrepass() const { return Material->UsesPrepass(); }

		reflected kmeta(onchange=OnMaterialPathChanged)
		String MaterialAssetPath;
		void OnMaterialPathChanged(const String& newPath);

	private:
		RefPtr<TMaterial> Material;
	};
}