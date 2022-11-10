#include "StaticMeshComponent.h"
#include "Tools/MeshLoader.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"

namespace ke
{

	StaticMeshComponent::StaticMeshComponent(RefPtr<StaticMesh> InStaticMesh)
		: m_StaticMesh(InStaticMesh)
	{
		SetStaticMesh(InStaticMesh);
	}

	StaticMeshComponent::StaticMeshComponent(RefPtr<IVertexBuffer> InVertexBuffer, RefPtr<IIndexBuffer> InIndexBuffer)
		: m_StaticMesh(StaticMesh::New(InVertexBuffer, InIndexBuffer))
	{
	}

	StaticMeshComponent::StaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices)
		: m_StaticMesh(StaticMesh::New(Vertices, InIndices))
	{
	}

	StaticMeshComponent::StaticMeshComponent(const Array<TStaticMeshSection>& Sections)
		: m_StaticMesh(StaticMesh::New(Sections))
	{
	}

	void StaticMeshComponent::SetStaticMesh(RefPtr<StaticMesh> NewMesh)
	{
		m_StaticMesh = NewMesh;

		if (m_StaticMesh->IsLoadedFromAsset())
		{
			if (!Asset)
			{
				Asset = Await(AssetManager::Get()->FindAssetNode(NewMesh->GetParentAssetPath()));
			}
		}
	}

	void StaticMeshComponent::Asset_Set(AssetTreeNode* pAsset)
	{
		if (!pAsset || pAsset == Asset)
		{
			return;
		}

		if (auto mesh = MeshLoader::Get()->LoadStaticMesh(pAsset->GetPath(), true))
		{
			m_StaticMesh = mesh;
		}
		Asset = pAsset;
	}
}