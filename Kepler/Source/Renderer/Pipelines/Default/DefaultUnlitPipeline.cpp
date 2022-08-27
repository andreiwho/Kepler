#include "DefaultUnlitPipeline.h"

namespace Kepler
{
	TDefaultUnlitPipeline::TDefaultUnlitPipeline()
		: TGraphicsPipeline()
	{
		TRef<TShader> Shader = LoadHLSLShader("EngineShaders/Core/DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	TGraphicsPipelineConfiguration TDefaultUnlitPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = true;
		Configuration.DepthStencil.DepthAccess = EDepthBufferAccess::Read | EDepthBufferAccess::Write;
		return Configuration;
	}

}