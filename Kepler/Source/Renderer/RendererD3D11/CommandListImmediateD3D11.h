#pragma once
#include "D3D11Common.h"
#include "Renderer/Elements/CommandList.h"
#include "VertexBufferD3D11.h"

namespace Kepler
{
	class TSwapChain;

	class TCommandListImmediateD3D11 : public TCommandListImmediate
	{
		static TCommandListImmediateD3D11* Instance;

	public:
		TCommandListImmediateD3D11(ID3D11DeviceContext4* InContext);
		~TCommandListImmediateD3D11();

		virtual void StartDrawingToSwapChainImage(TSwapChain* SwapChain) override;
		virtual void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]) override;
		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) override;

		static TCommandListImmediateD3D11* Get() { return CHECKED(Instance); }

		virtual void BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset) override;
		virtual void BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset) override;
		virtual void BindVertexBuffers(const TDynArray<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const TDynArray<u32>& Offsets) override;
		virtual void BindShader(TRef<TShader> Shader) override;
		virtual void BindPipeline(TRef<TGraphicsPipeline> Pipeline) override;
		virtual void DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset) override;
		virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) override;
		virtual void SetScissor(float X, float Y, float Width, float Height) override;
		virtual void* MapBuffer(TBuffer* Buffer) override;
		virtual void UnmapBuffer(TBuffer* Buffer) override;
		virtual void BindParamBuffers(TRef<TParamBuffer> ParamBufer, u32 Slot) override;
		virtual void BindParamBuffers(TDynArray<TRef<TParamBuffer>> ParamBufer, u32 Slot) override;

	private:
		ID3D11DeviceContext4* Context{};
		ID3D11VertexShader* BoundVertexShader = nullptr;
		ID3D11PixelShader* BoundPixelShader = nullptr;
		ID3D11ComputeShader* BoundComputeShader = nullptr;
		TGraphicsPipeline* BoundGraphicsPipeline = nullptr;
	};
}