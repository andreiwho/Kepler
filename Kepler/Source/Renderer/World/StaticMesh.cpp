#include "StaticMesh.h"
#include "Renderer/RenderGlobals.h"

namespace Kepler
{

	TStaticMesh::TStaticMesh(TRef<TVertexBuffer> InVertexBuffer, TRef<TIndexBuffer> InIndexBuffer)
		:	VertexBuffer(InVertexBuffer)
		,	IndexBuffer(InIndexBuffer)
	{
	}

	TStaticMesh::TStaticMesh(const TDynArray<TStaticMeshVertex>& Vertices, const TDynArray<u32>& InIndices)
	{
		CHECK(!IsRenderThread());

		SetVertices(Vertices);
		SetIndices(InIndices);
	}

	void TStaticMesh::SetVertices(const TDynArray<TStaticMeshVertex>& Vertices)
	{
		CHECK(!IsRenderThread());

		if (!VertexBuffer)
		{
			TRenderThread::Submit(
				[&Vertices, This = RefFromThis()]
				{
					This->VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Vertices));
				});
		}
	}

	void TStaticMesh::SetIndices(const TDynArray<u32>& Indices)
	{
		TRenderThread::Submit(
			[&Indices, This = RefFromThis()]
			{
				This->IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Indices));
			});
	}

}