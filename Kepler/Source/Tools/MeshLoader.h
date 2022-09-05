#pragma once
#include "Core/Core.h"

namespace Kepler
{
	class TStaticMesh;

	class TMeshLoader
	{
		static TMeshLoader* Instance;
	public:
		static TMeshLoader* Get() { return Instance; }

		TMeshLoader();
		~TMeshLoader();

		void ClearCache();

		TRef<TStaticMesh> LoadStaticMesh(const TString& MeshPath);
	};
}