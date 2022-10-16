#include "GraphicsPipeline.h"

#ifdef WIN32
# include "Renderer/RenderGlobals.h"
# include "Renderer/RendererD3D11/GraphicsPipelineHandleD3D11.h"
#else

#endif
#include "../HLSLShaderCompiler.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	IGraphicsPipeline::IGraphicsPipeline(RefPtr<IShader> pShader, const GraphicsPipelineConfig& config)
		: m_Handle(IGraphicsPipelineHandle::CreatePipelineHandle(pShader, config))
		, m_Shader(pShader)
		, m_Configuration(config)
	{
	}

	void IGraphicsPipeline::UploadParameters(RefPtr<ICommandListImmediate> pImmCmdList)
	{
	}

	void IGraphicsPipeline::Validate() const
	{
#ifdef ENABLE_DEBUG
		auto mappings = GetParamMapping();
		switch (GetDomain().Value)
		{
		case EPipelineDomain::Unlit:
		{
			CHECK(mappings->HasParam("Transform"));
			CHECK(mappings->HasParam("EntityId"));
		}
		break;
		case EPipelineDomain::Lit:
		{
			CHECK(mappings->HasParam("Transform"));
			CHECK(mappings->HasParam("NormalMatrix"));
			CHECK(mappings->HasParam("EntityId"));
		}
		break;
		case EPipelineDomain::Other:
		{
			break;
		}
		default:
			CRASH();
			break;
		}
#endif
	}

	RefPtr<IShader> IGraphicsPipeline::LoadHLSLShader(const TString& pShader, EShaderStageFlags stages)
	{
		RefPtr<THLSLShaderCompiler> pCompiler = THLSLShaderCompiler::CreateShaderCompiler();
		return pCompiler->CompileShader(pShader, stages);
	}

	void IGraphicsPipeline::DeferredInit(RefPtr<IShader> pShader, const GraphicsPipelineConfig& config)
	{
		m_Handle = IGraphicsPipelineHandle::CreatePipelineHandle(pShader, config);
		m_Shader = pShader;
		m_Configuration = config;
	}

	GraphicsPipelineCache* GraphicsPipelineCache::Instance;

	bool GraphicsPipelineCache::Exists(const TString& name) const
	{
		return m_Pipelines.Contains(name);
	}

	void GraphicsPipelineCache::Add(const TString& name, RefPtr<IGraphicsPipeline> pPipeline)
	{
		m_Pipelines.Insert(name, pPipeline);
	}

	RefPtr<IGraphicsPipeline> GraphicsPipelineCache::GetPipeline(const TString& name) const
	{
		CHECK(m_Pipelines.Contains(name));
		return m_Pipelines[name];
	}
}

// Some internals
namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	RefPtr<IGraphicsPipelineHandle> IGraphicsPipelineHandle::CreatePipelineHandle(RefPtr<IShader> pShader, const GraphicsPipelineConfig& config)
	{
		switch (GRenderAPI)
		{
		case ke::ERenderAPI::DirectX11:
			return MakeRef(New<TGraphicsPipelineHandleD3D11>(pShader, config));
			break;
		default:
			CHECK(false);
			break;
		}
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
}
