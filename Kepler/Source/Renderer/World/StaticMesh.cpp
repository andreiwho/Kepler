#include "StaticMesh.h"
#include "Renderer/RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{

	TStaticMesh::TStaticMesh(RefPtr<IVertexBuffer> pVertexBuffer, RefPtr<IIndexBuffer> pIndexBuffer)
		:	m_Sections({TInternalSection{pVertexBuffer, pIndexBuffer}})
	{
	}

	TStaticMesh::TStaticMesh(const Array<TStaticMeshVertex>& vertices, const Array<u32>& indices)
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

	TStaticMesh::TStaticMesh(const Array<TStaticMeshSection>& sections)
	{
		SetSections(sections);
	}

	void TStaticMesh::SetSections(const Array<TStaticMeshSection>& sections)
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
	}

}