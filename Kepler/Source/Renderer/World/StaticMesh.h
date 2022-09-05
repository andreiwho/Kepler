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

	struct TStaticMeshSection
	{
		TDynArray<TStaticMeshVertex> Vertices{};
		TDynArray<u32> Indices{};
	};

	class TStaticMesh : public TEnableRefFromThis<TStaticMesh>
	{
	public:
		TStaticMesh() = default;
		TStaticMesh(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer);
		TStaticMesh(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices);
		TStaticMesh(const TDynArray<TStaticMeshSection>& InSections);

	public:
		struct TInternalSection
		{
			TRef<TVertexBuffer> VertexBuffer{};
			TRef<TIndexBuffer> IndexBuffer{};
		};

		void SetSections(const TDynArray<TStaticMeshSection>& Sections);

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

		static TRef<TStaticMesh> New(const TDynArray<TStaticMeshSection>& Sections)
		{
			return MakeRef(Kepler::New<TStaticMesh>(Sections));
		}

		const TDynArray<TInternalSection>& GetSections() const { return Sections; }

	private:
		TDynArray<TInternalSection> Sections;
	
	};
}