#include "StaticMeshComponent.h"

namespace Kepler
{

	TStaticMeshComponent::TStaticMeshComponent(TRef<TStaticMesh> InStaticMesh)
		:	StaticMesh(InStaticMesh)
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer)
		:	StaticMesh(TStaticMesh::New(InVertexBuffer, InIndexBuffer))
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices)
		:	StaticMesh(TStaticMesh::New(Vertices, InIndices))
	{
	}

	TStaticMeshComponent::TStaticMeshComponent(const TDynArray<TStaticMeshSection>& Sections)
		: StaticMesh(TStaticMesh::New(Sections))
	{

	}

	void TStaticMeshComponent::SetStaticMesh(TRef<TStaticMesh> NewMesh)
	{
		StaticMesh = NewMesh;
	}
}