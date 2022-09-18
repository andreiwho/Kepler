#include "ScreenQuadPipeline.h"

namespace Kepler
{
	TScreenQuadPipeline::TScreenQuadPipeline(const TString& CustomShaderPath)
	{
		TRef<TShader> Shader = LoadHLSLShader(!CustomShaderPath.empty() ? CustomShaderPath : "EngineShaders://DefaultScreenQuad.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	TGraphicsPipelineConfiguration TScreenQuadPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = false;
		Configuration.DepthStencil.DepthAccess = EDepthBufferAccess::Read | EDepthBufferAccess::Write;

		Configuration.ParamMapping = Shader->GetReflection()->ParamMapping;

		return Configuration;
	}

}