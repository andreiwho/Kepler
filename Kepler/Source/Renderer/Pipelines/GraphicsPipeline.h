#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Elements/VertexLayout.h"
#include "Renderer/Elements/Shader.h"
#include "Core/Malloc.h"
#include "ParamPack.h"

namespace Kepler
{
	class TCommandListImmediate;

	// What pipeline needs
	// - Input layout.
	// - Uniform mapping
	// - Bunch of states (DS, RS, IA, VI, etc)

	struct TGraphicsPipelineConfiguration
	{
		ERenderPassId RenderPassMask{ ERenderPassId::Geometry };
		
		struct
		{
			EPrimitiveTopology Topology = EPrimitiveTopology::TriangleList;
			TVertexLayout VertexLayout{};
		} VertexInput;

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

		TRef<TPipelineParamMapping> ParamMapping;
	};

	class TGraphicsPipelineHandle : public TRefCounted
	{
	public:
		static TRef<TGraphicsPipelineHandle> CreatePipelineHandle(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);
	};

	class TGraphicsPipeline : public TRefCounted
	{
	public:
		TGraphicsPipeline() = default;
		TGraphicsPipeline(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

		virtual void UploadParameters(TRef<TCommandListImmediate> pImmCmdList);
	
		TRef<TShader> GetShader() const { return Shader; }
		TRef<TGraphicsPipelineHandle> GetHandle() const { return Handle; }

		TRef<TPipelineParamMapping> GetParamMapping() const { return Configuration.ParamMapping; }

	protected:
		static TRef<TShader> LoadHLSLShader(const TString& ShaderPath, EShaderStageFlags Stages);
		void DeferredInit(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

	private:
		TRef<TGraphicsPipelineHandle> Handle{};
		TRef<TShader> Shader{};
		TGraphicsPipelineConfiguration Configuration{};
	};
}