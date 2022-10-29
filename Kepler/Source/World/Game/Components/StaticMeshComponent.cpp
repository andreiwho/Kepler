#include "StaticMeshComponent.h"
#include "Tools/MeshLoader.h"

namespace ke
{

	StaticMeshComponent::StaticMeshComponent(RefPtr<StaticMesh> InStaticMesh)
		:	m_StaticMesh(InStaticMesh)
	{
		if (m_StaticMesh->IsLoadedFromAsset())
		{
			StaticMeshPath = m_StaticMesh->GetParentAssetPath();
		}
	}

	StaticMeshComponent::StaticMeshComponent(RefPtr<IVertexBuffer> InVertexBuffer, RefPtr<IIndexBuffer> InIndexBuffer)
		:	m_StaticMesh(StaticMesh::New(InVertexBuffer, InIndexBuffer))
	{
	}

	StaticMeshComponent::StaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices)
		:	m_StaticMesh(StaticMesh::New(Vertices, InIndices))
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
			StaticMeshPath = m_StaticMesh->GetParentAssetPath();
		}
	}

	void StaticMeshComponent::StaticMeshPathChanged(const String& newPath)
	{
		if (auto mesh = MeshLoader::Get()->LoadStaticMesh(newPath, true))
		{
			m_StaticMesh = mesh;
		}
	}

}