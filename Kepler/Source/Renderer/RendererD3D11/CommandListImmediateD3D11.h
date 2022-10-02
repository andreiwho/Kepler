#pragma once
#include "D3D11Common.h"
#include "Renderer/Elements/CommandList.h"
#include "VertexBufferD3D11.h"

namespace ke
{
	class TSwapChain;

	class TCommandListImmediateD3D11 : public TCommandListImmediate
	{
		static TCommandListImmediateD3D11* Instance;

	public:
		TCommandListImmediateD3D11(ID3D11DeviceContext4* InContext);
		~TCommandListImmediateD3D11();

		virtual void StartDrawingToSwapChainImage(TRef<TSwapChain> SwapChain, TRef<TDepthStencilTarget2D> DepthStencil = nullptr) override;
		virtual void ClearSwapChainImage(TRef<TSwapChain> SwapChain, float4 ClearColor) override;
		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) override;

		static TCommandListImmediateD3D11* Get() { return CHECKED(Instance); }
		inline ID3D11DeviceContext4* GetContext() const { return Context; }

		// Command list interface
		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) override;
		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) override;
		virtual void BindVertexBuffers(const TDynArray<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const TDynArray<u32>& Offsets) override;
		virtual void BindShader(TRef<TShader> Shader) override;
		virtual void BindSamplers(TRef<TPipelineSamplerPack> Samplers, u32 Slot = 0) override;
		virtual void ClearSamplers(u32 Slot = 0) override;
		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) override;
		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) override;
		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) override;
		virtual void SetScissor(float X, float Y, float Width, float Height) override;
		virtual void BindParamBuffers(TRef<TParamBuffer> ParamBufer, u32 Slot) override;
		virtual void BindParamBuffers(TDynArray<TRef<TParamBuffer>> ParamBuffers, u32 Slot) override;
		virtual void StartDrawingToRenderTargets(TRef<TRenderTarget2D> RenderTarget, TRef<TDepthStencilTarget2D> DepthStencil) override;
		virtual void StartDrawingToRenderTargets(const TDynArray<TRef<TRenderTarget2D>>& RenderTargets, TRef<TDepthStencilTarget2D> DepthStencil = nullptr) override;
		virtual void ClearRenderTarget(TRef<TRenderTarget2D> Target, float4 Color) override;
		virtual void ClearDepthTarget(TRef<TDepthStencilTarget2D> Target, bool bCleanStencil = false) override;

		// CommandListImmediate interface
		virtual void* MapBuffer(TRef<TBuffer> Buffer) override;
		virtual void UnmapBuffer(TRef<TBuffer> Buffer) override;
		virtual void Transfer(TRef<TImage2D> Into, usize X, usize Y, usize Width, usize Height, TRef<TDataBlob> Data) override;
		virtual void Transfer(TRef<TTransferBuffer> From, TRef<TBuffer> To, usize DstOffset, usize SrcOffset, usize Size) override;

		virtual void BeginDebugEvent(const char* Name) override;
		virtual void EndDebugEvent() override;

		virtual void* MapImage2D(TRef<TImage2D> Image, usize& OutAlignment) override;
		virtual void UnmapImage2D(TRef<TImage2D> Image) override;

	private:
		ID3D11DeviceContext4* Context{};
		ID3DUserDefinedAnnotation* AnnotationInterface{};
		ID3D11VertexShader* BoundVertexShader = nullptr;
		ID3D11PixelShader* BoundPixelShader = nullptr;
		ID3D11ComputeShader* BoundComputeShader = nullptr;
		TGraphicsPipeline* BoundGraphicsPipeline = nullptr;
	};
}