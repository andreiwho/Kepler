#pragma once
#include "Core/Core.h"
#include "Renderer/World/Material.h"

namespace ke
{
	class TMaterialLoader
	{
	private:
		static TMaterialLoader* Instance;

	public:
		TMaterialLoader();
		~TMaterialLoader();

		void ClearLoadedMaterialCache();

		RefPtr<TMaterial> LoadMaterial(const TString& Path, bool bForce = false);

		inline static TMaterialLoader* Get() { return Instance; }
	};
}