#pragma once
#include "Renderer/Elements/ParamBuffer.h"
#include "D3D11Common.h"

namespace ke
{
	class TParamBufferD3D11 : public TParamBuffer
	{
	public:
		TParamBufferD3D11(TRef<TPipelineParamMapping> Params);
		~TParamBufferD3D11();

		virtual void RT_UploadToGPU(TRef<class TCommandListImmediate> pImmContext) override;

		virtual void* GetNativeHandle() const override { return Buffer; }
	private:
		ID3D11Buffer* Buffer{ nullptr };
	};
}