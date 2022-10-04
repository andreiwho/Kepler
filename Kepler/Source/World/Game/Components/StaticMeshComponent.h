#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"

namespace ke
{
	class TStaticMeshComponent
	{
	public:
		TStaticMeshComponent() = default;
		TStaticMeshComponent(TRef<TStaticMesh> InStaticMesh);
		TStaticMeshComponent(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer);
		TStaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices);
		TStaticMeshComponent(const Array<TStaticMeshSection>& Sections);

		inline TRef<TStaticMesh> GetStaticMesh() const
		{
			return StaticMesh;
		}

		void SetStaticMesh(TRef<TStaticMesh> NewMesh);

	private:
		TRef<TStaticMesh> StaticMesh{};
	};
}