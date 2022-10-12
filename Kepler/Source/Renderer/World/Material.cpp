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

	void TMaterial::RT_Update(TRef<class GraphicsCommandListImmediate> pImmCmd)
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
		switch (m_Pipeline->GetDomain().Value)
		{
		case EPipelineDomain::Lit:
		{
			matrix4x4 matrix = glm::transpose(transform.GenerateWorldMatrix());
			matrix3x3 normal = glm::transpose(transform.GenerateNormalMatrix());
			m_ParamBuffer->Write("Transform", &matrix);
			m_ParamBuffer->Write("NormalMatrix", &matrix);
		}
		break;
		case EPipelineDomain::Unlit:
		{
			matrix4x4 matrix = glm::transpose(transform.GenerateWorldMatrix());
			m_ParamBuffer->Write("Transform", &matrix);
		}
		break;
		default:
			break;
		}
	}

	// BIG TODO: Use global renderer buffer in shaders to represent the camera state
	void TMaterial::WriteCamera(MathCamera camera)
	{
		matrix4x4 viewProj = glm::transpose(camera.GenerateViewProjectionMatrix());
		m_ParamBuffer->Write("ViewProjection", &viewProj);
	}

	void TMaterial::WriteId(i32 id)
	{
		m_ParamBuffer->Write("EntityId", &id);
	}
}