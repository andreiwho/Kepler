#include "MeshLoader.h"
#include "Renderer/World/StaticMesh.h"
#include <tiny_gltf.h>
#include "Core/Filesystem/FileUtils.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMeshLoader);

	TMeshLoader* TMeshLoader::Instance = nullptr;

	TMeshLoader::TMeshLoader()
	{
		Instance = this;
	}

	TMeshLoader::~TMeshLoader()
	{
		ClearCache();
	}

	void TMeshLoader::ClearCache()
	{

	}

	TRef<TStaticMesh> TMeshLoader::LoadStaticMesh(const TString& MeshPath)
	{
		return nullptr;
	}

}