#include "GraphicsPipeline.h"

#ifdef WIN32
# include "Renderer/RenderGlobals.h"
# include "Renderer/RendererD3D11/GraphicsPipelineHandleD3D11.h"
#else

#endif
#include "../HLSLShaderCompiler.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TGraphicsPipeline::TGraphicsPipeline(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Config)
		: Handle(TGraphicsPipelineHandle::CreatePipelineHandle(InShader, Config))
		, Shader(InShader)
		, Configuration(Config)
	{
	}

	void TGraphicsPipeline::UploadParameters(TRef<TCommandListImmediate> pImmCmdList)
	{
	}

	TRef<TShader> TGraphicsPipeline::LoadHLSLShader(const std::string& Shader, EShaderStageFlags Stages)
	{
		TRef<THLSLShaderCompiler> Compiler = THLSLShaderCompiler::CreateShaderCompiler();
		return Compiler->CompileShader(Shader, Stages);
	}

}

// Some internals
namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TRef<TGraphicsPipelineHandle> TGraphicsPipelineHandle::CreatePipelineHandle(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config)
	{
		switch (GRenderAPI)
		{
		case Kepler::ERenderAPI::DirectX11:
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
