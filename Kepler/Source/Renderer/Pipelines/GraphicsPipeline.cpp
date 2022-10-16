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
	IGraphicsPipeline::IGraphicsPipeline(RefPtr<IShader> InShader, const TGraphicsPipelineConfiguration& Config)
		: Handle(TGraphicsPipelineHandle::CreatePipelineHandle(InShader, Config))
		, Shader(InShader)
		, Configuration(Config)
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

	RefPtr<IShader> IGraphicsPipeline::LoadHLSLShader(const TString& Shader, EShaderStageFlags Stages)
	{
		RefPtr<THLSLShaderCompiler> Compiler = THLSLShaderCompiler::CreateShaderCompiler();
		return Compiler->CompileShader(Shader, Stages);
	}

	void IGraphicsPipeline::DeferredInit(RefPtr<IShader> InShader, const TGraphicsPipelineConfiguration& InConfiguration)
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

	void TGraphicsPipelineCache::Add(const TString& Name, RefPtr<IGraphicsPipeline> Pipeline)
	{
		Pipelines.Insert(Name, Pipeline);
	}

	RefPtr<IGraphicsPipeline> TGraphicsPipelineCache::GetPipeline(const TString& Name) const
	{
		CHECK(Pipelines.Contains(Name));
		return Pipelines[Name];
	}
}

// Some internals
namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	RefPtr<TGraphicsPipelineHandle> TGraphicsPipelineHandle::CreatePipelineHandle(RefPtr<IShader> Shader, const TGraphicsPipelineConfiguration& Config)
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
