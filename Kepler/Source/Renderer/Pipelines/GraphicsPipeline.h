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

	struct GraphicsPipelineConfig
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

	class IGraphicsPipelineHandle : public IntrusiveRefCounted
	{
	public:
		static RefPtr<IGraphicsPipelineHandle> CreatePipelineHandle(RefPtr<IShader> pShader, const GraphicsPipelineConfig& config);
	};

	class IGraphicsPipeline;

	class GraphicsPipelineCache
	{
		static GraphicsPipelineCache* Instance;
	public:
		static GraphicsPipelineCache* Get() { return Instance; }
		GraphicsPipelineCache() { Instance = this; }

		bool Exists(const String& name) const;
		void Add(const String& name, RefPtr<IGraphicsPipeline> pPipeline);
		RefPtr<IGraphicsPipeline> GetPipeline(const String& name) const;

	private:
		Map<String, RefPtr<IGraphicsPipeline>> m_Pipelines;
	};

	class IGraphicsPipeline : public IntrusiveRefCounted
	{
	public:
		IGraphicsPipeline() = default;
		IGraphicsPipeline(RefPtr<IShader> pShader, const GraphicsPipelineConfig& config);

		virtual void UploadParameters(RefPtr<ICommandListImmediate> pImmCmdList);

		RefPtr<IShader> GetShader() const { return m_Shader; }
		RefPtr<IGraphicsPipelineHandle> GetHandle() const { return m_Handle; }

		RefPtr<PipelineParamMapping> GetParamMapping() const { return m_Configuration.ParamMapping; }
		inline bool UsesPrepass() const { return m_Configuration.bUsePrepass; }
		inline EPipelineDomain GetDomain() const { return m_Configuration.Domain; }
		void Validate() const;

	protected:
		static RefPtr<IShader> LoadHLSLShader(const String& shaderPath, EShaderStageFlags stages);
		void DeferredInit(RefPtr<IShader> pShader, const GraphicsPipelineConfig& configuration);

	private:
		RefPtr<IGraphicsPipelineHandle> m_Handle{};
		RefPtr<IShader> m_Shader{};
		GraphicsPipelineConfig m_Configuration{};
	};
}