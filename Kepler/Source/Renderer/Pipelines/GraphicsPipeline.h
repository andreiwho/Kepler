#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Elements/VertexLayout.h"
#include "Renderer/Elements/Shader.h"
#include "Core/Malloc.h"
#include "ParamPack.h"

namespace ke
{
	class GraphicsCommandListImmediate;

	// What pipeline needs
	// - Input layout.
	// - Uniform mapping
	// - Bunch of states (DS, RS, IA, VI, etc)

	struct EPipelineDomain
	{
		enum EValue : u8
		{
			Undefined,
			Unlit,
			Lit,
		} Value{};

		EPipelineDomain() = default;
		EPipelineDomain(EValue value) : Value(value) {}
		inline operator EValue() const { return Value; }

		inline bool IsUndefined() const { return Value != Undefined; }
	};

	struct TGraphicsPipelineConfiguration
	{
		ERenderPassId RenderPassMask{ ERenderPassId::PrePass | ERenderPassId::Geometry };
		EPipelineDomain Domain{ EPipelineDomain::Unlit };

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
			bool bRasterDisabled = false;
		} Rasterizer;

		struct
		{
			bool bDepthEnable = true;
			EDepthBufferAccess DepthAccess = EDepthBufferAccess::Write;
			bool bStencilEnable = false;
			EStencilBufferAccess StencilAccess = EStencilBufferAccess::None;
			EDepthComparissonMode DepthFunc = EDepthComparissonMode::Less;
		} DepthStencil;
		bool bUsePrepass = true;

		TRef<TPipelineParamMapping> ParamMapping;
	};

	class TGraphicsPipelineHandle : public IntrusiveRefCounted
	{
	public:
		static TRef<TGraphicsPipelineHandle> CreatePipelineHandle(TRef<TShader> Shader, const TGraphicsPipelineConfiguration& Config);
	};

	class TGraphicsPipeline;
	class TGraphicsPipelineCache
	{
		static TGraphicsPipelineCache* Instance;
	public:
		static TGraphicsPipelineCache* Get() { return Instance; }
		TGraphicsPipelineCache() { Instance = this; }

		bool Exists(const TString& Name) const;
		void Add(const TString& Name, TRef<TGraphicsPipeline> Pipeline);
		TRef<TGraphicsPipeline> GetPipeline(const TString& Name) const;

	private:
		Map<TString, TRef<TGraphicsPipeline>> Pipelines;
	};

	class TGraphicsPipeline : public IntrusiveRefCounted
	{
	public:
		TGraphicsPipeline() = default;
		TGraphicsPipeline(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

		virtual void UploadParameters(TRef<GraphicsCommandListImmediate> pImmCmdList);

		TRef<TShader> GetShader() const { return Shader; }
		TRef<TGraphicsPipelineHandle> GetHandle() const { return Handle; }

		TRef<TPipelineParamMapping> GetParamMapping() const { return Configuration.ParamMapping; }
		inline bool UsesPrepass() const { return Configuration.bUsePrepass; }
		inline EPipelineDomain GetDomain() const { return Configuration.Domain; }
		void Validate() const;

	protected:
		static TRef<TShader> LoadHLSLShader(const TString& ShaderPath, EShaderStageFlags Stages);
		void DeferredInit(TRef<TShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

	private:
		TRef<TGraphicsPipelineHandle> Handle{};
		TRef<TShader> Shader{};
		TGraphicsPipelineConfiguration Configuration{};
	};
}