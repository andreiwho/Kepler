#include "StaticMesh.h"
#include "Renderer/RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{

	TStaticMesh::TStaticMesh(TRef<TVertexBuffer> pVertexBuffer, TRef<TIndexBuffer> pIndexBuffer)
		:	m_Sections({TInternalSection{pVertexBuffer, pIndexBuffer}})
	{
	}

	TStaticMesh::TStaticMesh(const TDynArray<TStaticMeshVertex>& vertices, const TDynArray<u32>& indices)
	{
		CHECK(!IsRenderThread());

		TInternalSection section;
		Await(TRenderThread::Submit([&section, vertices, indices]
			{
				section.VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(vertices));
				section.IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(indices));
			}));
		m_Sections.EmplaceBack(std::move(section));
	}

	TStaticMesh::TStaticMesh(const TDynArray<TStaticMeshSection>& sections)
	{
		SetSections(sections);
	}

	void TStaticMesh::SetSections(const TDynArray<TStaticMeshSection>& sections)
	{
		m_Sections.Clear();
		m_Sections.Reserve(sections.GetLength());
		for (const auto& section : sections)
		{
			TInternalSection outSection;
			Await(TRenderThread::Submit([&section, &outSection]
				{
					outSection.VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(section.Vertices));
					outSection.IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(section.Indices));
				}));
			m_Sections.EmplaceBack(std::move(outSection));
		}
	}

}