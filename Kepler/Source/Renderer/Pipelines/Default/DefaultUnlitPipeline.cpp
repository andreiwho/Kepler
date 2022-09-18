#include "DefaultUnlitPipeline.h"

namespace Kepler
{
	TDefaultUnlitPipeline::TDefaultUnlitPipeline(const TString& CustomShaderPath)
	{
		TRef<TShader> Shader = LoadHLSLShader(!CustomShaderPath.empty() ? CustomShaderPath : "EngineShaders://DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
		TGraphicsPipelineConfiguration Configuration = CreateConfiguration(Shader);
		DeferredInit(Shader, Configuration);
	}

	namespace
	{
		struct TConstants
		{
			matrix4x4 mViewProj;
			matrix4x4 mWorld;
			i32 EntityId;
		};
	}

	TGraphicsPipelineConfiguration TDefaultUnlitPipeline::CreateConfiguration(TRef<TShader> Shader)
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;

		Configuration.DepthStencil.bDepthEnable = true;
		Configuration.DepthStencil.DepthAccess = EDepthBufferAccess::Read | EDepthBufferAccess::Write;

		Configuration.ParamMapping = Shader->GetReflection()->ParamMapping;

		return Configuration;
	}

}