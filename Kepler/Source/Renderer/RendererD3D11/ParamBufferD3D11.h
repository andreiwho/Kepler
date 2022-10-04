#pragma once
#include "Renderer/Elements/ParamBuffer.h"
#include "D3D11Common.h"
#include "Renderer/LowLevelRenderer.h"
#include <array>

namespace ke
{
	class TParamBufferD3D11 : public TParamBuffer
	{
	public:
		TParamBufferD3D11(TRef<TPipelineParamMapping> Params);
		~TParamBufferD3D11();

		virtual void RT_UploadToGPU(TRef<class GraphicsCommandListImmediate> pImmContext) override;

		virtual void* GetNativeHandle() const override { return Buffer[TLowLevelRenderer::Get()->GetFrameIndex()]; }
		void* GetNextFrameHandle() const { return Buffer[TLowLevelRenderer::Get()->GetNextFrameIndex()]; }
	private:
		std::array<ID3D11Buffer*, TLowLevelRenderer::m_SwapChainFrameCount> Buffer{ nullptr };
	};
}