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

	class TStaticMesh : public TEnableRefFromThis<TStaticMesh>
	{
	public:
		TStaticMesh() = default;
		TStaticMesh(RefPtr<TVertexBuffer> pVertexBuffer, RefPtr<TIndexBuffer> pIndexBuffer);
		TStaticMesh(const Array<TStaticMeshVertex>& vertices, const Array<u32>& indices);
		TStaticMesh(const Array<TStaticMeshSection>& sections);

	public:
		struct TInternalSection
		{
			RefPtr<TVertexBuffer> VertexBuffer{};
			RefPtr<TIndexBuffer> IndexBuffer{};
		};

		void SetSections(const Array<TStaticMeshSection>& sections);

		static RefPtr<TStaticMesh> New()
		{
			return MakeRef(ke::New<TStaticMesh>());
		}

		static RefPtr<TStaticMesh> New(RefPtr<TVertexBuffer> pVertexBuffer, RefPtr<TIndexBuffer> pIndexBuffer)
		{
			return MakeRef(ke::New<TStaticMesh>(pVertexBuffer, pIndexBuffer));
		}

		static RefPtr<TStaticMesh> New(const Array<TStaticMeshVertex>& pVertices, const Array<u32>& pIndices)
		{
			return MakeRef(ke::New<TStaticMesh>(pVertices, pIndices));
		}

		static RefPtr<TStaticMesh> New(const Array<TStaticMeshSection>& sections)
		{
			return MakeRef(ke::New<TStaticMesh>(sections));
		}

		const Array<TInternalSection>& GetSections() const { return m_Sections; }

	private:
		Array<TInternalSection> m_Sections;
	
	};
}