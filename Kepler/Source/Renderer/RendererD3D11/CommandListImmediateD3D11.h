#pragma once
#include "D3D11Common.h"
#include "Renderer/Elements/CommandList.h"
#include "VertexBufferD3D11.h"

namespace ke { class TParamBufferD3D11; }

namespace ke
{
	class ISwapChain;

	class GraphicsCommandListImmediateD3D11 : public ICommandListImmediate
	{
		static GraphicsCommandListImmediateD3D11* Instance;

	public:
		GraphicsCommandListImmediateD3D11(ID3D11DeviceContext4* InContext);
		~GraphicsCommandListImmediateD3D11();

		virtual void StartDrawingToSwapChainImage(RefPtr<ISwapChain> pSwapChain, RefPtr<IDepthStencilTarget2D> pDepthStencil = nullptr) override;
		virtual void ClearSwapChainImage(RefPtr<ISwapChain> SwapChain, float4 ClearColor) override;
		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) override;

		static GraphicsCommandListImmediateD3D11* Get() { return CHECKED(Instance); }
		inline ID3D11DeviceContext4* GetContext() const { return Context; }

		// Command list interface
		virtual void BindVertexBuffers(RefPtr<IVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) override;
		virtual void BindVertexBuffers(RefPtr<IVertexBufferDynamic> pBuffer) override;
		virtual void BindIndexBuffer(RefPtr<IIndexBuffer> IndexBuffer, u32 Offset) override;
		virtual void BindVertexBuffers(const Array<RefPtr<IVertexBuffer>>& VertexBuffers, u32 StartSlot, const Array<u32>& Offsets) override;
		virtual void BindShader(RefPtr<IShader> Shader) override;
		virtual void BindSamplers(RefPtr<PipelineSamplerPack> Samplers, u32 Slot = 0) override;
		virtual void ClearSamplers(u32 Slot = 0) override;
		virtual void BindPipeline(RefPtr<IGraphicsPipeline> Pipeline) override;
		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) override;
		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) override;
		virtual void SetScissor(float X, float Y, float Width, float Height) override;
		virtual void BindParamBuffers(RefPtr<IParamBuffer> ParamBufer, u32 Slot) override;
		virtual void BindParamBuffers(Array<RefPtr<IParamBuffer>> ParamBuffers, u32 Slot) override;
		virtual void StartDrawingToRenderTargets(RefPtr<IRenderTarget2D> RenderTarget, RefPtr<IDepthStencilTarget2D> DepthStencil) override;
		virtual void StartDrawingToRenderTargets(const Array<RefPtr<IRenderTarget2D>>& RenderTargets, RefPtr<IDepthStencilTarget2D> DepthStencil = nullptr) override;
		virtual void ClearRenderTarget(RefPtr<IRenderTarget2D> Target, float4 Color) override;
		virtual void ClearDepthTarget(RefPtr<IDepthStencilTarget2D> Target, bool bCleanStencil = false) override;

		// CommandListImmediate interface
		virtual void* MapBuffer(RefPtr<IBuffer> Buffer) override;
		virtual void UnmapBuffer(RefPtr<IBuffer> Buffer) override;
		virtual void Transfer(RefPtr<IImage2D> Into, usize X, usize Y, usize Width, usize Height, RefPtr<IAsyncDataBlob> Data) override;
		virtual void Transfer(RefPtr<ITransferBuffer> From, RefPtr<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

		virtual void BeginDebugEvent(const char* Name) override;
		virtual void EndDebugEvent() override;

		virtual void* MapImage2D(RefPtr<IImage2D> Image, usize& OutAlignment) override;
		virtual void UnmapImage2D(RefPtr<IImage2D> Image) override;

		// Bindings for the d3d11 buffers
		void* MapParamBuffer_NextFrame(RefPtr<TParamBufferD3D11> Buffer);
		void UnmapParamBuffer_NextFrame(RefPtr<TParamBufferD3D11> Buffer);

	private:
		ID3D11DeviceContext4* Context{};
		ID3DUserDefinedAnnotation* AnnotationInterface{};
		ID3D11VertexShader* BoundVertexShader = nullptr;
		ID3D11PixelShader* BoundPixelShader = nullptr;
		ID3D11ComputeShader* BoundComputeShader = nullptr;
		IGraphicsPipeline* BoundGraphicsPipeline = nullptr;
	};
}