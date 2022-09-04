#pragma once
#include "../GraphicsPipeline.h"

namespace Kepler
{
	class TDefaultUnlitPipeline : public TGraphicsPipeline
	{
	public:
		TDefaultUnlitPipeline(const TString& CustomShaderPath = {});

		static TGraphicsPipelineConfiguration CreateConfiguration(TRef<TShader> Shader);
	};
}