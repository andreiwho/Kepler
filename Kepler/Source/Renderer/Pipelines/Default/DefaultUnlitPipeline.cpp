#include "DefaultUnlitPipeline.h"

namespace Kepler
{
	TDefaultUnlitPipeline::TDefaultUnlitPipeline(const TString& CustomShaderPath)
	{
		TRef<TShader> Shader = LoadHLSLShader(!CustomShaderPath.empty() ? CustomShaderPath : "EngineShaders://DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	struct TWorldViewProj
	{
		matrix4x4 mViewProj;
		matrix4x4 mWorld;
	};

	TGraphicsPipelineConfiguration TDefaultUnlitPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = true;
		Configuration.DepthStencil.DepthAccess = EDepthBufferAccess::Read | EDepthBufferAccess::Write;

		Configuration.ParamMapping = TPipelineParamMapping::New();
		Configuration.ParamMapping->AddParam("ViewProjection", 0, 0, EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);
		Configuration.ParamMapping->AddParam("Transform", offsetof(TWorldViewProj, mWorld), 0, EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);
		Configuration.ParamMapping->AddTextureSampler("Albedo", EShaderStageFlags::Pixel, 0);

		return Configuration;
	}

}