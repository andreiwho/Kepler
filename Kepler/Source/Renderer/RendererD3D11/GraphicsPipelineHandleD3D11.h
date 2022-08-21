#pragma once
#include "D3D11Common.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"

namespace Kepler
{
	class TGraphicsPipelineHandleD3D11 : public TGraphicsPipelineHandle
	{
	public:
		TGraphicsPipelineHandleD3D11(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);
		~TGraphicsPipelineHandleD3D11();

	private:
		void SetupRasterizer(const TGraphicsPipelineConfiguration& Config);
		void SetupDepthStencil(const TGraphicsPipelineConfiguration& Config);
		void SetupInputLayout(TRef<TShader> Shader);

	private:
		ID3D11RasterizerState* RasterState{};
		ID3D11DepthStencilState* DepthStencil{};
		ID3D11InputLayout* InputLayout{};
	};
}