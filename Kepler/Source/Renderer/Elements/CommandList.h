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

namespace ke
{
	class TSwapChain;
	class RenderTarget2D;
	class DepthStencilTarget2D;

	class GraphicsCommandList : public TEnableRefFromThis<GraphicsCommandList>
	{
	public:
		virtual ~GraphicsCommandList() = default;

		virtual void StartDrawingToSwapChainImage(TRef<TSwapChain> pSwapChain, TRef<DepthStencilTarget2D> pDepthStencil = nullptr) = 0;

		virtual void ClearSwapChainImage(TRef<TSwapChain> SwapChain, float4 ClearColor) = 0;
		
		// A version for the single buffer (called like this for optimization reasons)
		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) = 0;

		// A version for multiple buffers (called like this for optimization reasons)
		virtual void BindVertexBuffers(const Array<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const Array<u32>& Offsets) = 0;

		virtual void BindVertexBuffers(TRef<DynamicVertexBuffer> pBuffer) = 0;

		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) = 0;

		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) = 0;

		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) = 0;

		virtual void BindShader(TRef<TShader> Shader) = 0;

		virtual void BindSamplers(TRef<TPipelineSamplerPack> Samplers, u32 Slot = 0) = 0;

		virtual void ClearSamplers(u32 Slot = 0) = 0;

		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) = 0;

		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) = 0;
		
		virtual void SetScissor(float X, float Y, float Width, float Height) = 0;

		inline bool HasPipelineStateSetup() const { return m_bHasAttachedPipeline; }

		virtual void BindParamBuffers(TRef<TParamBuffer> ParamBufer, u32 Slot) = 0;
		
		virtual void BindParamBuffers(Array<TRef<TParamBuffer>> ParamBuffer, u32 Slot) = 0;

		virtual void StartDrawingToRenderTargets(TRef<RenderTarget2D> RenderTarget, TRef<DepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void StartDrawingToRenderTargets(const Array<TRef<RenderTarget2D>>& RenderTargets, TRef<DepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void ClearRenderTarget(TRef<RenderTarget2D> Target, float4 Color) = 0;

		virtual void ClearDepthTarget(TRef<DepthStencilTarget2D> Target, bool bCleanStencil = false) = 0;

		virtual void BeginDebugEvent(const char* Name) = 0;

		virtual void EndDebugEvent() = 0;
		
	protected:
		bool m_bHasAttachedPipeline = false;
	};

	class GraphicsCommandListImmediate : public GraphicsCommandList
	{
	public:
		virtual void* MapBuffer(TRef<IBuffer> Buffer) = 0;

		virtual void* MapImage2D(TRef<TImage2D> Image, usize& OutAlignment) = 0;

		virtual void UnmapImage2D(TRef<TImage2D> Image) = 0;

		virtual void UnmapBuffer(TRef<IBuffer> Buffer) = 0;

		virtual void Transfer(TRef<TTransferBuffer> From, TRef<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		virtual void Transfer(TRef<TImage2D> Into, usize X, usize Y, usize Width, usize Height, TRef<AsyncDataBlob> Data) = 0;
	};
}