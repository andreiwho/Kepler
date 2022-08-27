#pragma once
#include "../GraphicsPipeline.h"

namespace Kepler
{
	class TScreenQuadPipeline : public TGraphicsPipeline
	{
	public:
		TScreenQuadPipeline();

		static TGraphicsPipelineConfiguration CreateConfiguration(TRef<TShader> Shader);
	};
}