#pragma once
#include "Renderer/World/Material.h"

namespace ke
{
	class TMaterialComponent
	{
	public:
		TMaterialComponent() = default;
		TMaterialComponent(TRef<TMaterial> InMaterial);

		inline TRef<TMaterial> GetMaterial() { return Material; }
		void SetMaterial(TRef<TMaterial> InMaterial);

		inline const TString& GetMaterialParentAssetPath() const
		{
			return Material->GetParentAssetPath();
		}

		inline bool UsesPrepass() const { return Material->UsesPrepass(); }

	private:
		TRef<TMaterial> Material;
	};
}