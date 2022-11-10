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
	class ISwapChain;
	class IRenderTarget2D;
	class IDepthStencilTarget2D;
	class ITextureSampler2D;

	class IGfxCommandList : public EnableRefPtrFromThis<IGfxCommandList>
	{
	public:
		virtual ~IGfxCommandList() = default;

		virtual void StartDrawingToSwapChainImage(RefPtr<ISwapChain> pSwapChain, RefPtr<IDepthStencilTarget2D> pDepthStencil = nullptr) = 0;

		virtual void ClearSwapChainImage(RefPtr<ISwapChain> pSwapChain, float4 clearColor) = 0;
		
		// A version for the single buffer (called like this for optimization reasons)
		virtual void BindVertexBuffers(RefPtr<IVertexBuffer> pVertexBuffer, u32 startSlot, u32 offset) = 0;

		// A version for multiple buffers (called like this for optimization reasons)
		virtual void BindVertexBuffers(const Array<RefPtr<IVertexBuffer>>& ppVertexBuffers, u32 startSlot, const Array<u32>& offsets) = 0;

		virtual void BindVertexBuffers(RefPtr<IVertexBufferDynamic> pBuffer) = 0;

		virtual void BindIndexBuffer(RefPtr<IIndexBuffer> pIndexBuffer, u32 offset) = 0;

		virtual void Draw(u32 vertexCount, u32 baseVertexIndex) = 0;

		virtual void DrawIndexed(u32 indexCount, u32 baseIndexOffset, u32 baseVertexOffset) = 0;

		virtual void BindShader(RefPtr<IShader> pShader) = 0;

		virtual void BindSamplers(RefPtr<PipelineSamplerPack> pSamplers, u32 slot = 0) = 0;

		virtual void ClearSamplers(u32 slot = 0) = 0;

		virtual void BindPipeline(RefPtr<IGraphicsPipeline> pPipeline) = 0;

		virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
		
		virtual void SetScissor(float x, float y, float width, float height) = 0;

		inline bool HasPipelineStateSetup() const { return m_bHasAttachedPipeline; }

		virtual void BindParamBuffers(RefPtr<IParamBuffer> pParamBufer, u32 slot) = 0;
		
		virtual void BindParamBuffers(Array<RefPtr<IParamBuffer>> ppParamBuffers, u32 slot) = 0;

		virtual void StartDrawingToRenderTargets(RefPtr<IRenderTarget2D> pRenderTarget, RefPtr<IDepthStencilTarget2D> pDepthStencil = nullptr) = 0;

		virtual void StartDrawingToRenderTargets(const Array<RefPtr<IRenderTarget2D>>& ppRenderTargets, RefPtr<IDepthStencilTarget2D> pDepthStencil = nullptr) = 0;

		virtual void ClearRenderTarget(RefPtr<IRenderTarget2D> pTarget, float4 color) = 0;

		virtual void ClearDepthTarget(RefPtr<IDepthStencilTarget2D> pTarget, bool bCleanStencil = false) = 0;

		virtual void BeginDebugEvent(const char* pName) = 0;

		virtual void EndDebugEvent() = 0;
		
	protected:
		bool m_bHasAttachedPipeline = false;
	};

	class ICommandListImmediate : public IGfxCommandList
	{
	public:
		virtual void* MapBuffer(RefPtr<IBuffer> pBuffer) = 0;

		virtual void* MapImage2D(RefPtr<IImage2D> pImage, usize& outAlignment) = 0;

		virtual void UnmapImage2D(RefPtr<IImage2D> pImage) = 0;

		virtual void UnmapBuffer(RefPtr<IBuffer> pBuffer) = 0;

		virtual void Transfer(RefPtr<ITransferBuffer> pFrom, RefPtr<IBuffer> pTo, usize dstOffset, usize srcOffset, usize size) = 0;

		virtual void Transfer(RefPtr<IImage2D> pInto, usize x, usize y, usize width, usize height, RefPtr<IAsyncDataBlob> pData) = 0;

		virtual void GenerateMipmaps(RefPtr<ITextureSampler2D> pImage) = 0;
	};
}