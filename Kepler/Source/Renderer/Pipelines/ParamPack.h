#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	class TPipelineParam
	{
	public:
		TPipelineParam() = default;
		TPipelineParam(usize InOffset, usize InSize, EShaderInputType InType = EShaderInputType::Custom) 
			:	Offset(InOffset)
			,	Size(InSize)
			,	Type(InType)
		{}

		inline usize GetOffset() const { return Offset; }
		inline usize GetSize() const { return Size; }
		inline EShaderInputType GetType() const { return Type; }

	private:
		usize Offset{};
		usize Size{};
		EShaderInputType Type{EShaderInputType::Custom};
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
	private:
		TChaoticMap<TString, TPipelineParam> Params;
		EShaderStageFlags ParamShaderStages{0};
		
		TChaoticMap<TString, u32> Samplers;
		EShaderStageFlags SamplerShaderStages{ 0 };
	};

	//////////////////////////////////////////////////////////////////////////
	class TPipelineParamPack : public TRefCounted
	{
	public:
		TPipelineParamPack(TRef<TPipelineParamMapping> Mapping);

		[[deprecated]] inline bool IsCompiled() const { return bIsCompiled; }

		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param) && IsCompiled());

			const auto& ParamRef = Params->GetParams()[Param];
			memcpy(CPUData.GetData() + ParamRef.GetOffset(), Data, ParamRef.GetSize());
		}
		
		template<typename T>
		T& GetParam(const TString& Param)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param) && IsCompiled());
			return *reinterpret_cast<T*>(CPUData.GetData() + Params->GetParams().Find(Param)->GetOffset());
		}

		template<typename T>
		const T& GetParam(const TString& Param) const
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param) && IsCompiled());
			return *reinterpret_cast<const T*>(CPUData.GetData() + Params->GetParams().Find(Param)->GetOffset());
		}

		const ubyte* GetDataPointer() const { return CPUData.GetData(); }
		usize GetDataSize() const { return CPUData.GetLength(); }
		inline EShaderStageFlags GetShaderStages() const { return Params->GetParamShaderStages(); }

	private:
		TRef<TPipelineParamMapping> Params;
		TDynArray<ubyte> CPUData;
		bool bIsCompiled = false;
	};

	//////////////////////////////////////////////////////////////////////////
	class TTextureSampler2D;
	class TPipelineSamplerPack : public TRefCounted
	{
	public:
		TPipelineSamplerPack(TRef<TPipelineParamMapping> Mapping);

		void Write(const TString& Name, TRef<TTextureSampler2D> Data);

		TRef<TTextureSampler2D> GetSampler(const TString& Name);

		const TDynArray<TRef<TTextureSampler2D>>& GetSamplers() const { return Samplers; }
		
	private:
		TRef<TPipelineParamMapping> Params;
		TDynArray<TRef<TTextureSampler2D>> Samplers;
	};
}