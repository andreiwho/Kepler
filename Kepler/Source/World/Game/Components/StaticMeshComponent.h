#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"
#include "EntityComponent.h"

namespace ke
{
	class StaticMeshComponent : public EntityComponent
	{
	public:
		StaticMeshComponent() = default;
		StaticMeshComponent(RefPtr<StaticMesh> InStaticMesh);
		StaticMeshComponent(RefPtr<IVertexBuffer> InVertexBuffer, RefPtr<IIndexBuffer> InIndexBuffer);
		StaticMeshComponent(const Array<TStaticMeshVertex>& Vertices, const Array<u32>& InIndices);
		StaticMeshComponent(const Array<TStaticMeshSection>& Sections);

		inline RefPtr<StaticMesh> GetStaticMesh() const
		{
			return m_StaticMesh;
		}

		void SetStaticMesh(RefPtr<StaticMesh> NewMesh);

	private:
		RefPtr<StaticMesh> m_StaticMesh{};
	};
}