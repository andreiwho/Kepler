#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Elements/CommandList.h"

namespace Kepler
{
	// What pipeline needs
	// - Input layout.
	// - Uniform mapping
	// - Bunch of states (DS, RS, IA, VI, etc)

	struct TGraphicsPipelineConfiguration
	{
		ERenderPassId RenderPassMask{ ERenderPassId::Geometry };
	
		struct
		{
			EPrimitiveFillMode FillMode = EPrimitiveFillMode::Solid;
			EPrimitiveCullMode CullMode = EPrimitiveCullMode::Back;
			bool bEnableScissor = false;
		} Rasterizer;

		struct
		{
			bool bDepthEnable = true;
			EDepthBufferAccess DepthAccess = EDepthBufferAccess::None;
			bool bStencilEnable = false;
			EStencilBufferAccess StencilAccess = EStencilBufferAccess::None;
		} DepthStencil;
	};

	class TGraphicsPipelineHandle : public TRefCounted
	{
	public:
		static TRef<TGraphicsPipelineHandle> CreatePipelineHandle(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);
	};

	class TGraphicsPipeline : public TRefCounted
	{
	public:
		TGraphicsPipeline(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

		virtual void UploadParameters(TRef<TCommandListImmediate> pImmCmdList);;

	private:
		TRef<TGraphicsPipelineHandle> Handle;
		TRef<TShader> Shader;
		TGraphicsPipelineConfiguration Configuration{};
	};
}