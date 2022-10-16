#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Elements/VertexLayout.h"
#include "Renderer/Elements/Shader.h"
#include "Core/Malloc.h"
#include "ParamPack.h"

namespace ke
{
	class ICommandListImmediate;

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
			Other,
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
			VertexLayout VertexLayout{};
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

		RefPtr<PipelineParamMapping> ParamMapping;
	};

	class TGraphicsPipelineHandle : public IntrusiveRefCounted
	{
	public:
		static RefPtr<TGraphicsPipelineHandle> CreatePipelineHandle(RefPtr<IShader> Shader, const TGraphicsPipelineConfiguration& Config);
	};

	class IGraphicsPipeline;
	class TGraphicsPipelineCache
	{
		static TGraphicsPipelineCache* Instance;
	public:
		static TGraphicsPipelineCache* Get() { return Instance; }
		TGraphicsPipelineCache() { Instance = this; }

		bool Exists(const TString& Name) const;
		void Add(const TString& Name, RefPtr<IGraphicsPipeline> Pipeline);
		RefPtr<IGraphicsPipeline> GetPipeline(const TString& Name) const;

	private:
		Map<TString, RefPtr<IGraphicsPipeline>> Pipelines;
	};

	class IGraphicsPipeline : public IntrusiveRefCounted
	{
	public:
		IGraphicsPipeline() = default;
		IGraphicsPipeline(RefPtr<IShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

		virtual void UploadParameters(RefPtr<ICommandListImmediate> pImmCmdList);

		RefPtr<IShader> GetShader() const { return Shader; }
		RefPtr<TGraphicsPipelineHandle> GetHandle() const { return Handle; }

		RefPtr<PipelineParamMapping> GetParamMapping() const { return Configuration.ParamMapping; }
		inline bool UsesPrepass() const { return Configuration.bUsePrepass; }
		inline EPipelineDomain GetDomain() const { return Configuration.Domain; }
		void Validate() const;

	protected:
		static RefPtr<IShader> LoadHLSLShader(const TString& ShaderPath, EShaderStageFlags Stages);
		void DeferredInit(RefPtr<IShader> InShader, const TGraphicsPipelineConfiguration& Configuration);

	private:
		RefPtr<TGraphicsPipelineHandle> Handle{};
		RefPtr<IShader> Shader{};
		TGraphicsPipelineConfiguration Configuration{};
	};
}