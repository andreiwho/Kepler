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
		TParamBufferD3D11(RefPtr<TPipelineParamMapping> Params);
		~TParamBufferD3D11();

		virtual void RT_UploadToGPU(RefPtr<class GraphicsCommandListImmediate> pImmContext) override;

		virtual void* GetNativeHandle() const override { return Buffer[LowLevelRenderer::Get()->GetFrameIndex()]; }
		void* GetNextFrameHandle() const { return Buffer[LowLevelRenderer::Get()->GetNextFrameIndex()]; }
	private:
		std::array<ID3D11Buffer*, LowLevelRenderer::m_SwapChainFrameCount> Buffer{ nullptr };
	};
}