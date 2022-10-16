#include "StaticMeshComponent.h"

namespace ke
{

	TStaticMeshComponent::TStaticMeshComponent(RefPtr<TStaticMesh> InStaticMesh)
		:	StaticMesh(InStaticMesh)
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(RefPtr<TVertexBuffer> InVertexBuffer, RefPtr<TIndexBuffer> InIndexBuffer)
		:	StaticMesh(TStaticMesh::New(InVertexBuffer, InIndexBuffer))
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices)
		:	StaticMesh(TStaticMesh::New(Vertices, InIndices))
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(const Array<TStaticMeshSection>& Sections)
		: StaticMesh(TStaticMesh::New(Sections))
	{

	}

	void TStaticMeshComponent::SetStaticMesh(RefPtr<TStaticMesh> NewMesh)
	{
		StaticMesh = NewMesh;
	}
}