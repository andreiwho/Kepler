#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace ke
{
#define OFFSET_PARAM_ARGS(type, field) offsetof(type, field), sizeof(type::field) 


	//////////////////////////////////////////////////////////////////////////
	class PipelineParam
	{
	public:
		PipelineParam() = default;
		PipelineParam(usize offset, usize size, EShaderInputType type = EShaderInputType::Custom)
			: m_Offset(offset)
			, m_Size(size)
			, m_Type(type)
		{}

		inline usize GetOffset() const { return m_Offset; }
		inline usize GetSize() const { return m_Size; }
		inline EShaderInputType GetType() const { return m_Type; }

	private:
		usize m_Offset{};
		usize m_Size{};
		EShaderInputType m_Type{ EShaderInputType::Custom };
	};

	//////////////////////////////////////////////////////////////////////////
	class PipelineParamMapping : public EnableRefPtrFromThis<PipelineParamMapping>
	{
	public:
		void AddParam(const String& name, usize offset, usize size, EShaderStageFlags stage, EShaderInputType type);
		void AddTextureSampler(const String& name, EShaderStageFlags stage, u32 reg);

		RefPtr<class TPipelineParamPack> CreateParamPack();
		RefPtr<class PipelineSamplerPack> CreateSamplerPack();

		inline const auto& GetParams() const { return m_Params; }
		inline EShaderStageFlags GetParamShaderStages() const { return m_ParamShaderStages; }

		inline const auto& GetSamplers() const { return m_Samplers; }
		inline EShaderStageFlags GetSamplerShaderStages() const { return m_SamplerShaderStages; }

		static RefPtr<PipelineParamMapping> New()
		{
			return MakeRef(ke::New<PipelineParamMapping>());
		}

		inline bool HasParam(const String& name) const { return m_Params.Contains(name); }

		inline bool HasParams() const { return !m_Params.IsEmpty(); }
		inline bool HasSamplers() const { return !m_Samplers.IsEmpty(); }

	private:
		Map<String, PipelineParam> m_Params;
		EShaderStageFlags m_ParamShaderStages{ 0 };

		Map<String, u32> m_Samplers;
		EShaderStageFlags m_SamplerShaderStages{ 0 };
	};

	//////////////////////////////////////////////////////////////////////////
	class TPipelineParamPack : public IntrusiveRefCounted
	{
	public:
		TPipelineParamPack(RefPtr<PipelineParamMapping> pMapping);

		template<typename T>
		void Write(const String& param, const T* pData)
		{
			CHECK(m_Params);
			CHECK(m_Params->GetParams().Contains(param));

			const auto& ParamRef = m_Params->GetParams()[param];
			memcpy(m_CPUData.GetData() + ParamRef.GetOffset() + (GetBufferIndex() * m_SinglePackStride), pData, ParamRef.GetSize());
		}

		template<typename T>
		T& GetParam(const String& param)
		{
			CHECK(m_Params);
			CHECK(m_Params->GetParams().Contains(param));
			return *reinterpret_cast<T*>(m_CPUData.GetData() + m_Params->GetParams().Find(param)->GetOffset() + (GetBufferIndex() * m_SinglePackStride));
		}

		template<typename T>
		const T& GetParam(const String& param) const
		{
			CHECK(m_Params);
			CHECK(m_Params->GetParams().Contains(param));
			return *reinterpret_cast<const T*>(m_CPUData.GetData() + m_Params->GetParams().Find(param)->GetOffset() + (GetBufferIndex() * m_SinglePackStride));
		}

		const ubyte* GetDataPointer() const { return m_CPUData.GetData() + (GetBufferIndex() * m_SinglePackStride); }
		usize GetDataSize() const { return m_SinglePackStride; }
		inline EShaderStageFlags GetShaderStages() const { return m_Params->GetParamShaderStages(); }

		inline bool HasParam(const String& name) const { return m_Params->HasParam(name); }

	private:
		static u8 GetBufferIndex() noexcept;

	private:
		RefPtr<PipelineParamMapping> m_Params;
		Array<ubyte> m_CPUData;
		bool m_bIsCompiled = false;
		usize m_SinglePackStride = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class ITextureSampler2D;
	class PipelineSamplerPack : public IntrusiveRefCounted
	{
	public:
		PipelineSamplerPack(RefPtr<PipelineParamMapping> pMapping);

		void Write(const String& name, RefPtr<ITextureSampler2D> pData);

		RefPtr<ITextureSampler2D> GetSampler(const String& name);

		RefPtr<PipelineParamMapping> GetParamMappings() const { return m_Params; }

		const Array<RefPtr<ITextureSampler2D>>& GetSamplers() const { return m_Samplers; }

	private:
		RefPtr<PipelineParamMapping> m_Params;
		Array<RefPtr<ITextureSampler2D>> m_Samplers;
	};
}