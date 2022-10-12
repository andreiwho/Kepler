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
	class TPipelineParamMapping : public TEnableRefFromThis<TPipelineParamMapping>
	{
	public:
		void AddParam(const TString& Name, usize Offset, usize Size, EShaderStageFlags Stage, EShaderInputType Type);
		void AddTextureSampler(const TString& Name, EShaderStageFlags Stage, u32 Register);

		TRef<class TPipelineParamPack> CreateParamPack();
		TRef<class TPipelineSamplerPack> CreateSamplerPack();

		inline const auto& GetParams() const { return Params; }
		inline EShaderStageFlags GetParamShaderStages() const { return ParamShaderStages; }

		inline const auto& GetSamplers() const { return Samplers; }
		inline EShaderStageFlags GetSamplerShaderStages() const { return SamplerShaderStages; }

		static TRef<TPipelineParamMapping> New()
		{
			return MakeRef(ke::New<TPipelineParamMapping>());
		}

		inline bool HasParam(const TString& name) const { return Params.Contains(name); }

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
		TPipelineParamPack(TRef<TPipelineParamMapping> Mapping);

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
		TRef<TPipelineParamMapping> Params;
		Array<ubyte> CPUData;
		bool bIsCompiled = false;
		usize SinglePackStride = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class TTextureSampler2D;
	class TPipelineSamplerPack : public IntrusiveRefCounted
	{
	public:
		TPipelineSamplerPack(TRef<TPipelineParamMapping> Mapping);

		void Write(const TString& Name, TRef<TTextureSampler2D> Data);

		TRef<TTextureSampler2D> GetSampler(const TString& Name);

		TRef<TPipelineParamMapping> GetParamMappings() const { return Params; }

		const Array<TRef<TTextureSampler2D>>& GetSamplers() const { return Samplers; }

	private:
		TRef<TPipelineParamMapping> Params;
		Array<TRef<TTextureSampler2D>> Samplers;
	};
}