#include "StaticMeshComponent.h"

namespace ke
{

	TStaticMeshComponent::TStaticMeshComponent(TRef<TStaticMesh> InStaticMesh)
		:	StaticMesh(InStaticMesh)
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer)
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

	void TStaticMeshComponent::SetStaticMesh(TRef<TStaticMesh> NewMesh)
	{
		StaticMesh = NewMesh;
	}
}