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
	};

	struct TStaticMeshSection
	{
		TDynArray<TStaticMeshVertex> Vertices{};
		TDynArray<u32> Indices{};
	};

	class TStaticMesh : public TEnableRefFromThis<TStaticMesh>
	{
	public:
		TStaticMesh() = default;
		TStaticMesh(TRef<TVertexBuffer> pVertexBuffer, TRef<TIndexBuffer> pIndexBuffer);
		TStaticMesh(const TDynArray<TStaticMeshVertex>& vertices, const TDynArray<u32>& indices);
		TStaticMesh(const TDynArray<TStaticMeshSection>& sections);

	public:
		struct TInternalSection
		{
			TRef<TVertexBuffer> VertexBuffer{};
			TRef<TIndexBuffer> IndexBuffer{};
		};

		void SetSections(const TDynArray<TStaticMeshSection>& sections);

		static TRef<TStaticMesh> New()
		{
			return MakeRef(ke::New<TStaticMesh>());
		}

		static TRef<TStaticMesh> New(TRef<TVertexBuffer> pVertexBuffer, TRef<TIndexBuffer> pIndexBuffer)
		{
			return MakeRef(ke::New<TStaticMesh>(pVertexBuffer, pIndexBuffer));
		}

		static TRef<TStaticMesh> New(const TDynArray<TStaticMeshVertex>& pVertices, const TDynArray<u32>& pIndices)
		{
			return MakeRef(ke::New<TStaticMesh>(pVertices, pIndices));
		}

		static TRef<TStaticMesh> New(const TDynArray<TStaticMeshSection>& sections)
		{
			return MakeRef(ke::New<TStaticMesh>(sections));
		}

		const TDynArray<TInternalSection>& GetSections() const { return m_Sections; }

	private:
		TDynArray<TInternalSection> m_Sections;
	
	};
}