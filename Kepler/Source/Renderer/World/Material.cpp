#include "Material.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{
	TMaterial::TMaterial(TRef<TGraphicsPipeline> pPipeline, const TString& parentAssetPath)
		: m_Pipeline(pPipeline), m_ParentAssetPath(parentAssetPath)
	{
		CHECK(!IsRenderThread());
		Await(TRenderThread::Submit(
			[This = RefFromThis()]
			{
				auto ParamMapping = This->GetPipeline()->GetParamMapping();
				if (ParamMapping)
				{
					This->m_ParamBuffer = TParamBuffer::New(ParamMapping);
					This->m_Samplers = ParamMapping->CreateSamplerPack();
				}
			}));
	}

	void TMaterial::RT_Update(TRef<class TCommandListImmediate> pImmCmd)
	{
		CHECK(IsRenderThread());
		m_ParamBuffer->RT_UploadToGPU(pImmCmd);
	}

	void TMaterial::WriteSampler(const TString& name, TRef<TTextureSampler2D> data)
	{
		m_Samplers->Write(name, data);
	}

	void TMaterial::WriteTransform(TWorldTransform transform)
	{
		matrix4x4 matrix = glm::transpose(transform.GenerateWorldMatrix());
		m_ParamBuffer->Write("Transform", &matrix);
	}

	// BIG TODO: Use global renderer buffer in shaders to represent the camera state
	void TMaterial::WriteCamera(TCamera camera)
	{
		matrix4x4 viewProj = glm::transpose(camera.GenerateViewProjectionMatrix());
		m_ParamBuffer->Write("ViewProjection", &viewProj);
	}

	void TMaterial::WriteId(i32 id)
	{
		m_ParamBuffer->Write("EntityId", &id);
	}
}