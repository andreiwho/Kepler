#pragma once
#include "Core/Core.h"
#include "Renderer/World/StaticMesh.h"

#include <assimp/matrix4x4.h>

class aiMesh;
class aiNode;
class aiScene;

namespace ke
{
	class MeshLoader
	{
		static MeshLoader* Instance;
	public:
		static MeshLoader* Get() { return Instance; }

		MeshLoader();
		~MeshLoader();

		void ClearCache();

		RefPtr<StaticMesh> LoadStaticMesh(const String& MeshPath, bool bTryOutputSingleSection = false);

	private:
		// For internal use only
		Array<TStaticMeshSection> LoadStaticMeshSections(const String& MeshPath, bool bTryOutputSingleSection = false);
		bool ProcessNode(const aiScene* pScene, const aiNode* pNode, Array<TStaticMeshSection>& OutSections);
		bool ProcessMesh(const aiScene* pScene, const aiMesh* pMesh, const aiMatrix4x4& ParentTransform, Array<TStaticMeshSection>& OutSections);
	};
}