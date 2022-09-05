#pragma once
#include "Core/Core.h"
#include "Renderer/World/StaticMesh.h"

#include <assimp/matrix4x4.h>

class aiMesh;
class aiNode;
class aiScene;

namespace Kepler
{
	class TMeshLoader
	{
		static TMeshLoader* Instance;
	public:
		static TMeshLoader* Get() { return Instance; }

		TMeshLoader();
		~TMeshLoader();

		void ClearCache();

		TDynArray<TStaticMeshSection> LoadStaticMeshSections(const TString& MeshPath);

	private:
		bool ProcessNode(const aiScene* pScene, const aiNode* pNode, TDynArray<TStaticMeshSection>& OutSections);
		bool ProcessMesh(const aiScene* pScene, const aiMesh* pMesh, const aiMatrix4x4& ParentTransform, TDynArray<TStaticMeshSection>& OutSections);
	};
}