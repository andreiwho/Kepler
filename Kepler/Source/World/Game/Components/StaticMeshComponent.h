#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"

namespace ke
{
	class TStaticMeshComponent
	{
	public:
		TStaticMeshComponent() = default;
		TStaticMeshComponent(RefPtr<TStaticMesh> InStaticMesh);
		TStaticMeshComponent(RefPtr<TVertexBuffer> InVertexBuffer, RefPtr<TIndexBuffer> InIndexBuffer);
		TStaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices);
		TStaticMeshComponent(const Array<TStaticMeshSection>& Sections);

		inline RefPtr<TStaticMesh> GetStaticMesh() const
		{
			return StaticMesh;
		}

		void SetStaticMesh(RefPtr<TStaticMesh> NewMesh);

	private:
		RefPtr<TStaticMesh> StaticMesh{};
	};
}