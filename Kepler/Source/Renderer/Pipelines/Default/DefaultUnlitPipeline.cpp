#include "DefaultUnlitPipeline.h"
#include "Math/Vector.h"

namespace Kepler
{
	TDefaultUnlitPipeline::TDefaultUnlitPipeline()
		: TGraphicsPipeline(LoadHLSLShader("Kepler/Shaders/Core/DefaultUnlit.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel),
			CreateConfiguration())
	{
	}

	TGraphicsPipelineConfiguration TDefaultUnlitPipeline::CreateConfiguration()
	{
		TGraphicsPipelineConfiguration Configuration{};
		Configuration.VertexInput.VertexLayout.AddAttribute(0, "POSITION", EShaderInputType::Float3, 0);
		Configuration.VertexInput.VertexLayout.AddAttribute(0, "COLOR", EShaderInputType::Float3, sizeof(float3));

		Configuration.DepthStencil.bDepthEnable = false;
		return Configuration;
	}

}