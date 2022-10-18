#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"
#include "EntityComponent.h"

namespace ke
{
	class TStaticMeshComponent : public EntityComponent
	{
	public:
		TStaticMeshComponent() = default;
		TStaticMeshComponent(RefPtr<TStaticMesh> InStaticMesh);
		TStaticMeshComponent(RefPtr<IVertexBuffer> InVertexBuffer, RefPtr<IIndexBuffer> InIndexBuffer);
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