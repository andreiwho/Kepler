#include "Material.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"

namespace Kepler
{
	TMaterial::TMaterial(TRef<TGraphicsPipeline> InPipeline, const TString& InParentAssetPath)
		: Pipeline(InPipeline), ParentAssetPath(InParentAssetPath)
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

	void TMaterial::WriteTransform(TWorldTransform Transform)
	{
		matrix4x4 Matrix = glm::transpose(Transform.GenerateWorldMatrix());
		ParamBuffer->Write("Transform", &Matrix);
	}

	// BIG TODO: Use global renderer buffer in shaders to represent the camera state
	void TMaterial::WriteCamera(TCamera Camera)
	{
		matrix4x4 ViewProjection = glm::transpose(Camera.GenerateViewProjectionMatrix());
		ParamBuffer->Write("ViewProjection", &ViewProjection);
	}

}