#pragma once
#include "D3D11Common.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"

namespace ke
{
	class TGraphicsPipelineHandleD3D11 : public IGraphicsPipelineHandle
	{
	public:
		TGraphicsPipelineHandleD3D11(RefPtr<IShader> Shader, const GraphicsPipelineConfig& Config);
		~TGraphicsPipelineHandleD3D11();

		inline ID3D11RasterizerState* GetRasterState() const { return RasterState; }
		inline ID3D11DepthStencilState* GetDepthStencilState() const { return DepthStencil; }
		inline ID3D11InputLayout* GetInputLayout() const { return InputLayout; }
		inline D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return PrimitiveTopology; }

	private:
		void SetupRasterizer(const GraphicsPipelineConfig& Config);
		void SetupDepthStencil(const GraphicsPipelineConfig& Config);
		void SetupInputLayout(RefPtr<IShader> Shader, const GraphicsPipelineConfig& Config);

	private:
		ID3D11RasterizerState* RasterState{};
		ID3D11DepthStencilState* DepthStencil{};
		ID3D11InputLayout* InputLayout{};
		D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology{};
	};
}