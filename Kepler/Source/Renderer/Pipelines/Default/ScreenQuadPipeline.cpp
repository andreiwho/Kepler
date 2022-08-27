#include "ScreenQuadPipeline.h"

namespace Kepler
{
	TScreenQuadPipeline::TScreenQuadPipeline()
		: TGraphicsPipeline()
	{
		TRef<TShader> Shader = LoadHLSLShader("Kepler/Shaders/Core/ScreenQuad.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	TGraphicsPipelineConfiguration TScreenQuadPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = false;
		Configuration.DepthStencil.DepthAccess = EDepthBufferAccess::Read | EDepthBufferAccess::Write;
		return Configuration;
	}

}