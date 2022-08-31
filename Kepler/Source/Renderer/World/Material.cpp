#include "Material.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"

namespace Kepler
{
	TMaterial::TMaterial(TRef<TGraphicsPipeline> InPipeline)
		: Pipeline(InPipeline)
	{
		CHECK(!IsRenderThread());
		Await(TRenderThread::Submit(
			[This = RefFromThis()]
			{
				auto ParamMapping = This->GetPipeline()->GetParamMapping();
				if (ParamMapping)
				{
					This->ParamBuffer = TParamBuffer::New(ParamMapping);
					This->Samplers = ParamMapping->CreateSamplerPack();
				}
			}));
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