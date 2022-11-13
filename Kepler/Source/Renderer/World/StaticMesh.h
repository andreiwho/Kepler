#pragma once
#include "Core/Core.h"
#include "Renderer/Elements/VertexBuffer.h"
#include "Renderer/Elements/IndexBuffer.h"

namespace ke
{
	// TODO: Make standard vertex
	struct TStaticMeshVertex
	{
		float3 Position;
		float3 Color;
		float2 UV0;
		float3 Normal;
		float3 Tangent;
		float3 Bitangent;
	};

	struct TStaticMeshSection
	{
		Array<TStaticMeshVertex> Vertices{};
		Array<u32> Indices{};
	};

	class StaticMesh : public EnableRefPtrFromThis<StaticMesh>
	{
	public:
		StaticMesh() = default;
		StaticMesh(RefPtr<IVertexBuffer> pVertexBuffer, RefPtr<IIndexBuffer> pIndexBuffer);
		StaticMesh(const Array<TStaticMeshVertex>& vertices, const Array<u32>& indices);
		StaticMesh(const Array<TStaticMeshSection>& sections, const String& assetPath = "", bool bForcedSingleSection = false);

	public:
		struct TInternalSection
		{
			RefPtr<IVertexBuffer> VertexBuffer{};
			RefPtr<IIndexBuffer> IndexBuffer{};
		};

		void SetSections(const Array<TStaticMeshSection>& sections);

		static RefPtr<StaticMesh> New()
		{
			return MakeRef(ke::New<StaticMesh>());
		}

		static RefPtr<StaticMesh> New(RefPtr<IVertexBuffer> pVertexBuffer, RefPtr<IIndexBuffer> pIndexBuffer)
		{
			return MakeRef(ke::New<StaticMesh>(pVertexBuffer, pIndexBuffer));
		}

		static RefPtr<StaticMesh> New(const Array<TStaticMeshVertex>& pVertices, const Array<u32>& pIndices)
		{
			return MakeRef(ke::New<StaticMesh>(pVertices, pIndices));
		}

		static RefPtr<StaticMesh> New(const Array<TStaticMeshSection>& sections, const String& assetPath = "", bool bForcedSingleSection = false)
		{
			return MakeRef(ke::New<StaticMesh>(sections, assetPath, bForcedSingleSection));
		}
		const Array<TInternalSection>& GetSections() const { return m_Sections; }
		inline bool IsForcedSingleSectionOnLoad() const { return m_bForcedSingleSection; }
		inline const String& GetParentAssetPath() const& { return m_ParentAssetName; }
		inline bool IsLoadedFromAsset() const { return !m_ParentAssetName.empty(); }
		inline float3 GetBoundingBoxSize() const { return m_BoundingBox; }

	private:
		static float3 CalculateBoundingBox(const Array<TStaticMeshSection>& sections);

	private:
		Array<TInternalSection> m_Sections;
		bool m_bForcedSingleSection = false;
		String m_ParentAssetName{};
		float3 m_BoundingBox{};
	};
}