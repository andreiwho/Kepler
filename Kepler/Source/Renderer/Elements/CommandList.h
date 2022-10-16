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

		virtual void StartDrawingToSwapChainImage(RefPtr<TSwapChain> pSwapChain, RefPtr<DepthStencilTarget2D> pDepthStencil = nullptr) = 0;

		virtual void ClearSwapChainImage(RefPtr<TSwapChain> SwapChain, float4 ClearColor) = 0;
		
		// A version for the single buffer (called like this for optimization reasons)
		virtual void BindVertexBuffers(RefPtr<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) = 0;

		// A version for multiple buffers (called like this for optimization reasons)
		virtual void BindVertexBuffers(const Array<RefPtr<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const Array<u32>& Offsets) = 0;

		virtual void BindVertexBuffers(RefPtr<DynamicVertexBuffer> pBuffer) = 0;

		virtual void BindIndexBuffer(RefPtr<TIndexBuffer> IndexBuffer, u32 Offset) = 0;

		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) = 0;

		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) = 0;

		virtual void BindShader(RefPtr<TShader> Shader) = 0;

		virtual void BindSamplers(RefPtr<TPipelineSamplerPack> Samplers, u32 Slot = 0) = 0;

		virtual void ClearSamplers(u32 Slot = 0) = 0;

		virtual void BindPipeline(RefPtr<TGraphicsPipeline> Pipeline) = 0;

		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) = 0;
		
		virtual void SetScissor(float X, float Y, float Width, float Height) = 0;

		inline bool HasPipelineStateSetup() const { return m_bHasAttachedPipeline; }

		virtual void BindParamBuffers(RefPtr<TParamBuffer> ParamBufer, u32 Slot) = 0;
		
		virtual void BindParamBuffers(Array<RefPtr<TParamBuffer>> ParamBuffer, u32 Slot) = 0;

		virtual void StartDrawingToRenderTargets(RefPtr<RenderTarget2D> RenderTarget, RefPtr<DepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void StartDrawingToRenderTargets(const Array<RefPtr<RenderTarget2D>>& RenderTargets, RefPtr<DepthStencilTarget2D> DepthStencil = nullptr) = 0;

		virtual void ClearRenderTarget(RefPtr<RenderTarget2D> Target, float4 Color) = 0;

		virtual void ClearDepthTarget(RefPtr<DepthStencilTarget2D> Target, bool bCleanStencil = false) = 0;

		virtual void BeginDebugEvent(const char* Name) = 0;

		virtual void EndDebugEvent() = 0;
		
	protected:
		bool m_bHasAttachedPipeline = false;
	};

	class GraphicsCommandListImmediate : public GraphicsCommandList
	{
	public:
		virtual void* MapBuffer(RefPtr<IBuffer> Buffer) = 0;

		virtual void* MapImage2D(RefPtr<TImage2D> Image, usize& OutAlignment) = 0;

		virtual void UnmapImage2D(RefPtr<TImage2D> Image) = 0;

		virtual void UnmapBuffer(RefPtr<IBuffer> Buffer) = 0;

		virtual void Transfer(RefPtr<TTransferBuffer> From, RefPtr<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) = 0;

		virtual void Transfer(RefPtr<TImage2D> Into, usize X, usize Y, usize Width, usize Height, RefPtr<AsyncDataBlob> Data) = 0;
	};
}