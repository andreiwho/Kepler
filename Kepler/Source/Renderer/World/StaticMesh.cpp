#include "StaticMesh.h"
#include "Renderer/RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{

	TStaticMesh::TStaticMesh(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer)
		:	Sections({TInternalSection{InVertexBuffer, InIndexBuffer}})
	{
	}

	TStaticMesh::TStaticMesh(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices)
	{
		CHECK(!IsRenderThread());

		TInternalSection Section;
		Await(TRenderThread::Submit([&Section, Vertices, InIndices]
			{
				Section.VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Vertices));
				Section.IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(InIndices));
			}));
		Sections.EmplaceBack(std::move(Section));
	}

	TStaticMesh::TStaticMesh(const TDynArray<TStaticMeshSection>& InSections)
	{
		SetSections(InSections);
	}

	void TStaticMesh::SetSections(const TDynArray<TStaticMeshSection>& InSections)
	{
		Sections.Clear();
		Sections.Reserve(InSections.GetLength());
		for (const auto& Section : InSections)
		{
			TInternalSection OutSection;
			Await(TRenderThread::Submit([&Section, &OutSection]
				{
					OutSection.VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Section.Vertices));
					OutSection.IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Section.Indices));
				}));
			Sections.EmplaceBack(std::move(OutSection));
		}
	}

}