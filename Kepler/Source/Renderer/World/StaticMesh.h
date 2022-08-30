#pragma once
#include "Core/Core.h"
#include "Renderer/Elements/VertexBuffer.h"
#include "Renderer/Elements/IndexBuffer.h"

namespace Kepler
{
	struct TStaticMeshVertex
	{
		float3 Position;
		float3 Color;
		float2 UV0;
	};

	class TStaticMesh : public TEnableRefFromThis<TStaticMesh>
	{
	public:
		TStaticMesh() = default;
		TStaticMesh(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer);
		TStaticMesh(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices);

	public:

		void SetVertices(const TDynArray<TStaticMeshVertex>& Vertices);
		void SetIndices(const TDynArray<u32>& Indices);

		static TRef<TStaticMesh> New()
		{
			return MakeRef(Kepler::New<TStaticMesh>());
		}

		static TRef<TStaticMesh> New(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer)
		{
			return MakeRef(Kepler::New<TStaticMesh>(InVertexBuffer, InIndexBuffer));
		}

		static TRef<TStaticMesh> New(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices)
		{
			return MakeRef(Kepler::New<TStaticMesh>(Vertices, InIndices));
		}


		inline TRef<TVertexBuffer> GetVertexBuffer() const { return VertexBuffer; }
		inline TRef<TIndexBuffer> GetIndexBuffer() const { return IndexBuffer; }
		inline usize GetIndexCount() const { return IndexBuffer->GetCount(); }

	private:
		TRef<TVertexBuffer> VertexBuffer{};
		TRef<TIndexBuffer> IndexBuffer{};
	};
}