#include "DefaultUnlitPipeline.h"

namespace Kepler
{
	TDefaultUnlitPipeline::TDefaultUnlitPipeline()
		: TGraphicsPipeline()
	{
		TRef<TShader> Shader = LoadHLSLShader("Kepler/Shaders/Core/DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	TGraphicsPipelineConfiguration TDefaultUnlitPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = false;
		return Configuration;
	}

}