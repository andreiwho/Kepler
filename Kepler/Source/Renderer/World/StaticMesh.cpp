#include "StaticMesh.h"
#include "Renderer/RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{

	StaticMesh::StaticMesh(RefPtr<IVertexBuffer> pVertexBuffer, RefPtr<IIndexBuffer> pIndexBuffer)
		:	m_Sections({TInternalSection{pVertexBuffer, pIndexBuffer}})
	{
	}

	StaticMesh::StaticMesh(const Array<TStaticMeshVertex>& vertices, const Array<u32>& indices)
	{
		CHECK(!IsRenderThread());

		TInternalSection section;
		Await(TRenderThread::Submit([&section, vertices, indices]
			{
				section.VertexBuffer = IVertexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(vertices));
				section.IndexBuffer = IIndexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(indices));
			}));
		m_Sections.EmplaceBack(std::move(section));
	}

	StaticMesh::StaticMesh(const Array<TStaticMeshSection>& sections, const String& assetPath, bool bForcedSingleSection)
		:	m_ParentAssetName(assetPath)
		,	m_bForcedSingleSection(bForcedSingleSection)
	{
		SetSections(sections);
	}

	void StaticMesh::SetSections(const Array<TStaticMeshSection>& sections)
	{
		m_Sections.Clear();
		m_Sections.Reserve(sections.GetLength());
		for (const auto& section : sections)
		{
			TInternalSection outSection;
			Await(TRenderThread::Submit([&section, &outSection]
				{
					outSection.VertexBuffer = IVertexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(section.Vertices));
					outSection.IndexBuffer = IIndexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(section.Indices));
				}));
			m_Sections.EmplaceBack(std::move(outSection));
		}
		m_BoundingBox = CalculateBoundingBox(sections);
	}

	ke::float3 StaticMesh::CalculateBoundingBox(const Array<TStaticMeshSection>& sections)
	{
		float3 minExtent, maxExtent;
		minExtent = maxExtent = sections[0].Vertices[0].Position;

		for (const auto& section : sections)
		{
			for (const auto& vertex : section.Vertices)
			{
				if (vertex.Position.x < minExtent.x) minExtent.x = vertex.Position.x;
				if (vertex.Position.x > maxExtent.x) maxExtent.x = vertex.Position.x;

				if (vertex.Position.y < minExtent.y) minExtent.y = vertex.Position.y;
				if (vertex.Position.y > maxExtent.y) maxExtent.y = vertex.Position.y;


				if (vertex.Position.z < minExtent.z) minExtent.z = vertex.Position.z;
				if (vertex.Position.z > maxExtent.z) maxExtent.z = vertex.Position.z;
			}
		}
		return maxExtent - minExtent;
	}

}