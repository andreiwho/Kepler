#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"

namespace Kepler
{
	class TStaticMeshComponent
	{
	public:
		TStaticMeshComponent() = default;
		TStaticMeshComponent(TRef<TStaticMesh> InStaticMesh);
		TStaticMeshComponent(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer);
		TStaticMeshComponent(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices);
		TStaticMeshComponent(const TDynArray<TStaticMeshSection>& Sections);

		inline TRef<TStaticMesh> GetStaticMesh() const
		{
			return StaticMesh;
		}

		void SetStaticMesh(TRef<TStaticMesh> NewMesh);

	private:
		TRef<TStaticMesh> StaticMesh{};
	};
}