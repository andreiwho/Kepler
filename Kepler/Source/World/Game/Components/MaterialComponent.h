#pragma once
#include "Renderer/World/Material.h"
#include "EntityComponent.h"

namespace ke
{
	class TMaterialComponent : public EntityComponent
	{
	public:
		TMaterialComponent() = default;
		TMaterialComponent(RefPtr<TMaterial> InMaterial);

		inline RefPtr<TMaterial> GetMaterial() { return Material; }
		void SetMaterial(RefPtr<TMaterial> InMaterial);

		inline const TString& GetMaterialParentAssetPath() const
		{
			return Material->GetParentAssetPath();
		}

		inline bool UsesPrepass() const { return Material->UsesPrepass(); }

	private:
		RefPtr<TMaterial> Material;
	};
}