#pragma once
#include "D3D11Common.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"

namespace ke
{
	class TGraphicsPipelineHandleD3D11 : public TGraphicsPipelineHandle
	{
	public:
		TGraphicsPipelineHandleD3D11(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);
		~TGraphicsPipelineHandleD3D11();

		inline ID3D11RasterizerState* GetRasterState() const { return RasterState; }
		inline ID3D11DepthStencilState* GetDepthStencilState() const { return DepthStencil; }
		inline ID3D11InputLayout* GetInputLayout() const { return InputLayout; }
		inline D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return PrimitiveTopology; }

	private:
		void SetupRasterizer(const TGraphicsPipelineConfiguration& Config);
		void SetupDepthStencil(const TGraphicsPipelineConfiguration& Config);
		void SetupInputLayout(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);

	private:
		ID3D11RasterizerState* RasterState{};
		ID3D11DepthStencilState* DepthStencil{};
		ID3D11InputLayout* InputLayout{};
		D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology{};
	};
}