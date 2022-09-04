#pragma once
#include "../GraphicsPipeline.h"

namespace Kepler
{
	class TScreenQuadPipeline : public TGraphicsPipeline
	{
	public:
		TScreenQuadPipeline(const TString& CustomShaderPath = {});

		static TGraphicsPipelineConfiguration CreateConfiguration(TRef<TShader> Shader);
	};
}