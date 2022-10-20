#pragma once
#include "Renderer/World/Material.h"
#include "EntityComponent.h"

namespace ke
{
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

	private:
		RefPtr<TMaterial> Material;
	};
}