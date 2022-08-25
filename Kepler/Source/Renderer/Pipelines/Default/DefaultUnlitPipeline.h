#pragma once
#include "../GraphicsPipeline.h"

namespace Kepler
{
	class TDefaultUnlitPipeline : public TGraphicsPipeline
	{
	public:
		TDefaultUnlitPipeline();

		static TGraphicsPipelineConfiguration CreateConfiguration(TRef<TShader> Shader);
	};
}