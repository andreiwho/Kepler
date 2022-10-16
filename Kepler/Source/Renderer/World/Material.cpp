#include "Material.h"
#include "../RenderGlobals.h"
#include "Async/Async.h"

namespace ke
{
	TMaterial::TMaterial(RefPtr<IGraphicsPipeline> pPipeline, const TString& parentAssetPath)
		: m_Pipeline(pPipeline), m_ParentAssetPath(parentAssetPath)
	{
		CHECK(!IsRenderThread());
		Await(TRenderThread::Submit(
			[This = RefFromThis()]
			{
				auto ParamMapping = This->GetPipeline()->GetParamMapping();
				if (ParamMapping)
				{
					if (ParamMapping->HasParams())
					{
						This->m_ParamBuffer = IParamBuffer::New(ParamMapping);
					}

					if (ParamMapping->HasSamplers())
					{
						This->m_Samplers = ParamMapping->CreateSamplerPack();
					}
				}
			}));
	}

	void TMaterial::RT_Update(RefPtr<class ICommandListImmediate> pImmCmd)
	{
		CHECK(IsRenderThread());
		m_ParamBuffer->RT_UploadToGPU(pImmCmd);
	}

	void TMaterial::WriteSampler(const TString& name, RefPtr<ITextureSampler2D> data)
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