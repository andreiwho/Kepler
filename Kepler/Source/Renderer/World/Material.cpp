#include "Material.h"
#include "../RenderGlobals.h"

namespace Kepler
{
	TMaterial::TMaterial(TRef<TGraphicsPipeline> InPipeline, TRef<TPipelineParamMapping> InParamMapping)
		:	Pipeline(InPipeline)
	{
		CHECK(!IsRenderThread());
		TRenderThread::Submit(
			[This = RefFromThis(), InParamMapping]
			{
				This->ParamBuffer = TParamBuffer::New(InParamMapping);
				This->Samplers = InParamMapping->CreateSamplerPack();
			});
	}

	void TMaterial::RT_Update(TRef<class TCommandListImmediate> pImmCmd)
	{
		CHECK(IsRenderThread());
		ParamBuffer->RT_UploadToGPU(pImmCmd);
	}

	void TMaterial::WriteSampler(const TString& Name, TRef<TTextureSampler2D> Data)
	{
		Samplers->Write(Name, Data);
	}
}