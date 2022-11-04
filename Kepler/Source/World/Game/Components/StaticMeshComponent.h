#pragma once
#include "Core/Types.h"
#include "Renderer/World/StaticMesh.h"
#include "EntityComponent.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "StaticMeshComponent.gen.h"

namespace ke
{
	reflected class StaticMeshComponent : public EntityComponent
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

		String StaticMeshPath{};
		void StaticMeshPathChanged(const String& newPath);

		reflected kmeta(postchange = OnMeshAssetChanged, assettype=StaticMesh)
		AssetTreeNode* Asset {nullptr};
		void OnMeshAssetChanged(AssetTreeNode* pAsset);

	private:
		RefPtr<StaticMesh> m_StaticMesh{};
	};
}