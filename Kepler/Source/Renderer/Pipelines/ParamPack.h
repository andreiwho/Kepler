#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace ke
{
#define OFFSET_PARAM_ARGS(type, field) offsetof(type, field), sizeof(type::field) 


	//////////////////////////////////////////////////////////////////////////
	class TPipelineParam
	{
	public:
		TPipelineParam() = default;
		TPipelineParam(usize InOffset, usize InSize, EShaderInputType InType = EShaderInputType::Custom)
			: Offset(InOffset)
			, Size(InSize)
			, Type(InType)
		{}

		inline usize GetOffset() const { return Offset; }
		inline usize GetSize() const { return Size; }
		inline EShaderInputType GetType() const { return Type; }

	private:
		usize Offset{};
		usize Size{};
		EShaderInputType Type{ EShaderInputType::Custom };
	};

	//////////////////////////////////////////////////////////////////////////
	class PipelineParamMapping : public EnableRefPtrFromThis<PipelineParamMapping>
	{
	public:
		void AddParam(const TString& Name, usize Offset, usize Size, EShaderStageFlags Stage, EShaderInputType Type);
		void AddTextureSampler(const TString& Name, EShaderStageFlags Stage, u32 Register);

		RefPtr<class TPipelineParamPack> CreateParamPack();
		RefPtr<class PipelineSamplerPack> CreateSamplerPack();

		inline const auto& GetParams() const { return Params; }
		inline EShaderStageFlags GetParamShaderStages() const { return ParamShaderStages; }

		inline const auto& GetSamplers() const { return Samplers; }
		inline EShaderStageFlags GetSamplerShaderStages() const { return SamplerShaderStages; }

		static RefPtr<PipelineParamMapping> New()
		{
			return MakeRef(ke::New<PipelineParamMapping>());
		}

		inline bool HasParam(const TString& name) const { return Params.Contains(name); }

		inline bool HasParams() const { return !Params.IsEmpty(); }
		inline bool HasSamplers() const { return !Samplers.IsEmpty(); }

	private:
		Map<TString, TPipelineParam> Params;
		EShaderStageFlags ParamShaderStages{ 0 };

		Map<TString, u32> Samplers;
		EShaderStageFlags SamplerShaderStages{ 0 };
	};

	//////////////////////////////////////////////////////////////////////////
	class TPipelineParamPack : public IntrusiveRefCounted
	{
	public:
		TPipelineParamPack(RefPtr<PipelineParamMapping> Mapping);

		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param));

			const auto& ParamRef = Params->GetParams()[Param];
			memcpy(CPUData.GetData() + ParamRef.GetOffset() + (GetBufferIndex() * SinglePackStride), Data, ParamRef.GetSize());
		}

		template<typename T>
		T& GetParam(const TString& Param)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param));
			return *reinterpret_cast<T*>(CPUData.GetData() + Params->GetParams().Find(Param)->GetOffset() + (GetBufferIndex() * SinglePackStride));
		}

		template<typename T>
		const T& GetParam(const TString& Param) const
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param));
			return *reinterpret_cast<const T*>(CPUData.GetData() + Params->GetParams().Find(Param)->GetOffset() + (GetBufferIndex() * SinglePackStride));
		}

		const ubyte* GetDataPointer() const { return CPUData.GetData() + (GetBufferIndex() * SinglePackStride); }
		usize GetDataSize() const { return SinglePackStride; }
		inline EShaderStageFlags GetShaderStages() const { return Params->GetParamShaderStages(); }

		inline bool HasParam(const TString& name) const { return Params->HasParam(name); }

	private:
		static u8 GetBufferIndex() noexcept;

	private:
		RefPtr<PipelineParamMapping> Params;
		Array<ubyte> CPUData;
		bool bIsCompiled = false;
		usize SinglePackStride = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class ITextureSampler2D;
	class PipelineSamplerPack : public IntrusiveRefCounted
	{
	public:
		PipelineSamplerPack(RefPtr<PipelineParamMapping> Mapping);

		void Write(const TString& Name, RefPtr<ITextureSampler2D> Data);

		RefPtr<ITextureSampler2D> GetSampler(const TString& Name);

		RefPtr<PipelineParamMapping> GetParamMappings() const { return Params; }

		const Array<RefPtr<ITextureSampler2D>>& GetSamplers() const { return Samplers; }

	private:
		RefPtr<PipelineParamMapping> Params;
		Array<RefPtr<ITextureSampler2D>> Samplers;
	};
}