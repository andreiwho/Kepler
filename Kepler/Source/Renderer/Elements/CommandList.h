#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/DynArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"

namespace Kepler
{
	class TSwapChain;

	class TCommandList : public TRefCounted
	{
	public:
		virtual ~TCommandList() = default;

		virtual void StartDrawingToSwapChainImage(TSwapChain* SwapChain) = 0;
		virtual void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]) = 0;
		
		// A version for the single buffer (called like this for optimization reasons)
		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) = 0;

		// A version for multiple buffers (called like this for optimization reasons)
		virtual void BindVertexBuffers(const TDynArray<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const TDynArray<u32>& Offsets) = 0;

		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) = 0;

		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) = 0;

		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) = 0;

		virtual void BindShader(TRef<TShader> Shader) = 0;

		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) = 0;

		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) = 0;
		
		virtual void SetScissor(float X, float Y, float Width, float Height) = 0;


		inline bool HasPipelineStateSetup() const { return bHasAttachedPipeline; }
	protected:
		bool bHasAttachedPipeline = false;
	};

	class TCommandListImmediate : public TCommandList
	{
	};
}