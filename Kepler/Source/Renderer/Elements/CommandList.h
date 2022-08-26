#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/DynArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "ParamBuffer.h"
#include "Buffer.h"
#include "Image.h"
#include "Renderer/Pipelines/ParamPack.h"

namespace Kepler
{
	class TSwapChain;
	class TRenderTarget2D;
	class TDepthStencilTarget2D;

	class TCommandList : public TRefCounted
	{
	public:
		virtual ~TCommandList() = default;

		virtual void StartDrawingToSwapChainImage(TRef<TSwapChain> SwapChain, TRef<TDepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void ClearSwapChainImage(TRef<TSwapChain> SwapChain, float4 ClearColor) = 0;
		
		// A version for the single buffer (called like this for optimization reasons)
		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) = 0;

		// A version for multiple buffers (called like this for optimization reasons)
		virtual void BindVertexBuffers(const TDynArray<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const TDynArray<u32>& Offsets) = 0;

		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) = 0;

		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) = 0;

		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) = 0;

		virtual void BindShader(TRef<TShader> Shader) = 0;

		virtual void BindSamplers(TRef<TPipelineSamplerPack> Samplers, u32 Slot = 0) = 0;

		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) = 0;

		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) = 0;
		
		virtual void SetScissor(float X, float Y, float Width, float Height) = 0;

		inline bool HasPipelineStateSetup() const { return bHasAttachedPipeline; }

		virtual void BindParamBuffers(TRef<TParamBuffer> ParamBufer, u32 Slot) = 0;
		
		virtual void BindParamBuffers(TDynArray<TRef<TParamBuffer>> ParamBuffer, u32 Slot) = 0;

		virtual void StartDrawingToRenderTargets(TRef<TRenderTarget2D> RenderTarget, TRef<TDepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void StartDrawingToRenderTargets(const TDynArray<TRef<TRenderTarget2D>>& RenderTargets, TRef<TDepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void ClearRenderTarget(TRef<TRenderTarget2D> Target, float4 Color) = 0;

		virtual void ClearDepthTarget(TRef<TDepthStencilTarget2D> Target, bool bCleanStencil = false) = 0;
		
	protected:
		bool bHasAttachedPipeline = false;
	};

	class TCommandListImmediate : public TCommandList
	{
	public:
		virtual void* MapBuffer(TRef<TBuffer> Buffer) = 0;

		virtual void UnmapBuffer(TRef<TBuffer> Buffer) = 0;

		virtual void Transfer(TRef<TTransferBuffer> From, TRef<TBuffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		virtual void Transfer(TRef<TImage2D> Into, usize X, usize Y, usize Width, usize Height, TRef<TDataBlob> Data) = 0;
	};
}