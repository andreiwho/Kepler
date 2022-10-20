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
	}

}