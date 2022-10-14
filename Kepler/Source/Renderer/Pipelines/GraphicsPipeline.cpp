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
	TGraphicsPipeline::TGraphicsPipeline(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Config)
		: Handle(TGraphicsPipelineHandle::CreatePipelineHandle(InShader, Config))
		, Shader(InShader)
		, Configuration(Config)
	{
	}

	void TGraphicsPipeline::UploadParameters(TRef<GraphicsCommandListImmediate> pImmCmdList)
	{
	}

	void TGraphicsPipeline::Validate() const
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

	TRef<TShader> TGraphicsPipeline::LoadHLSLShader(const TString& Shader, EShaderStageFlags Stages)
	{
		TRef<THLSLShaderCompiler> Compiler = THLSLShaderCompiler::CreateShaderCompiler();
		return Compiler->CompileShader(Shader, Stages);
	}

	void TGraphicsPipeline::DeferredInit(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& InConfiguration)
	{
		Handle = TGraphicsPipelineHandle::CreatePipelineHandle(InShader, InConfiguration);
		Shader = InShader;
		Configuration = InConfiguration;
	}

	TGraphicsPipelineCache* TGraphicsPipelineCache::Instance;

	bool TGraphicsPipelineCache::Exists(const TString& Name) const
	{
		return Pipelines.Contains(Name);
	}

	void TGraphicsPipelineCache::Add(const TString& Name, TRef<TGraphicsPipeline> Pipeline)
	{
		Pipelines.Insert(Name, Pipeline);
	}

	TRef<TGraphicsPipeline> TGraphicsPipelineCache::GetPipeline(const TString& Name) const
	{
		CHECK(Pipelines.Contains(Name));
		return Pipelines[Name];
	}
}

// Some internals
namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	TRef<TGraphicsPipelineHandle> TGraphicsPipelineHandle::CreatePipelineHandle(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config)
	{
		switch (GRenderAPI)
		{
		case ke::ERenderAPI::DirectX11:
			return MakeRef(New<TGraphicsPipelineHandleD3D11>(Shader, Config));
			break;
		default:
			CHECK(false);
			break;
		}
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
}
