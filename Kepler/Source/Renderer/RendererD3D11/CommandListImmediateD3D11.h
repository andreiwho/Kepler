#pragma once
#include "D3D11Common.h"
#include "Renderer/Elements/CommandList.h"
#include "VertexBufferD3D11.h"

namespace ke { class TParamBufferD3D11; }

namespace ke
{
	class TSwapChain;

	class GraphicsCommandListImmediateD3D11 : public GraphicsCommandListImmediate
	{
		static GraphicsCommandListImmediateD3D11* Instance;

	public:
		GraphicsCommandListImmediateD3D11(ID3D11DeviceContext4* InContext);
		~GraphicsCommandListImmediateD3D11();

		virtual void StartDrawingToSwapChainImage(TRef<TSwapChain> pSwapChain, TRef<DepthStencilTarget2D> pDepthStencil = nullptr) override;
		virtual void ClearSwapChainImage(TRef<TSwapChain> SwapChain, float4 ClearColor) override;
		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) override;

		static GraphicsCommandListImmediateD3D11* Get() { return CHECKED(Instance); }
		inline ID3D11DeviceContext4* GetContext() const { return Context; }

		// Command list interface
		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) override;
		virtual void BindVertexBuffers(TRef<DynamicVertexBuffer> pBuffer) override;
		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) override;
		virtual void BindVertexBuffers(const Array<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const Array<u32>& Offsets) override;
		virtual void BindShader(TRef<TShader> Shader) override;
		virtual void BindSamplers(TRef<TPipelineSamplerPack> Samplers, u32 Slot = 0) override;
		virtual void ClearSamplers(u32 Slot = 0) override;
		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) override;
		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) override;
		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) override;
		virtual void SetScissor(float X, float Y, float Width, float Height) override;
		virtual void BindParamBuffers(TRef<TParamBuffer> ParamBufer, u32 Slot) override;
		virtual void BindParamBuffers(Array<TRef<TParamBuffer>> ParamBuffers, u32 Slot) override;
		virtual void StartDrawingToRenderTargets(TRef<RenderTarget2D> RenderTarget, TRef<DepthStencilTarget2D> DepthStencil) override;
		virtual void StartDrawingToRenderTargets(const Array<TRef<RenderTarget2D>>& RenderTargets, TRef<DepthStencilTarget2D> DepthStencil = nullptr) override;
		virtual void ClearRenderTarget(TRef<RenderTarget2D> Target, float4 Color) override;
		virtual void ClearDepthTarget(TRef<DepthStencilTarget2D> Target, bool bCleanStencil = false) override;

		// CommandListImmediate interface
		virtual void* MapBuffer(TRef<IBuffer> Buffer) override;
		virtual void UnmapBuffer(TRef<IBuffer> Buffer) override;
		virtual void Transfer(TRef<TImage2D> Into, usize X, usize Y, usize Width, usize Height, TRef<AsyncDataBlob> Data) override;
		virtual void Transfer(TRef<TTransferBuffer> From, TRef<IBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

		virtual void BeginDebugEvent(const char* Name) override;
		virtual void EndDebugEvent() override;

		virtual void* MapImage2D(TRef<TImage2D> Image, usize& OutAlignment) override;
		virtual void UnmapImage2D(TRef<TImage2D> Image) override;

		// Bindings for the d3d11 buffers
		void* MapParamBuffer_NextFrame(TRef<TParamBufferD3D11> Buffer);
		void UnmapParamBuffer_NextFrame(TRef<TParamBufferD3D11> Buffer);

	private:
		ID3D11DeviceContext4* Context{};
		ID3DUserDefinedAnnotation* AnnotationInterface{};
		ID3D11VertexShader* BoundVertexShader = nullptr;
		ID3D11PixelShader* BoundPixelShader = nullptr;
		ID3D11ComputeShader* BoundComputeShader = nullptr;
		TGraphicsPipeline* BoundGraphicsPipeline = nullptr;
	};
}