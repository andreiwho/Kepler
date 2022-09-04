#pragma once
#include "Core/Core.h"
#include "Renderer/World/Material.h"

namespace Kepler
{
	class TMaterialLoader
	{
	private:
		static TMaterialLoader* Instance;

	public:
		TMaterialLoader();
		~TMaterialLoader();

		void ClearLoadedMaterialCache();

		TRef<TMaterial> LoadMaterial(const TString& Path);

		inline static TMaterialLoader* Get() { return Instance; }
	};
}