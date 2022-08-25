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
		// You can leave the size as 0 to automatically deduce the size depending on type
		// Note: This doesn't work for 
		void AddParam(const TString& Name, usize Offset, usize Size, EShaderStageFlags Stage, EShaderInputType Type);
		TRef<class TPipelineParamPack> CreatePack();

		inline const auto& GetParams() const { return Params; }
		inline EShaderStageFlags GetShaderStages() const { return ShaderStages; }
	private:
		TChaoticMap<TString, TPipelineParam> Params;
		EShaderStageFlags ShaderStages{0};
	};

	//////////////////////////////////////////////////////////////////////////
	class TPipelineParamPack : public TRefCounted
	{
	public:
		TPipelineParamPack(TRef<TPipelineParamMapping> Mapping);

		inline bool IsCompiled() const { return bIsCompiled; }

		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param) && IsCompiled());

			const auto& ParamRef = Params->GetParams()[Param];
			memcpy(CPUData.GetData() + ParamRef.GetOffset(), Data, ParamRef.GetSize());
		}
		
		template<typename T>
		T* GetParam(const TString& Param)
		{
			CHECK(Params);
			CHECK(Params->GetParams().Contains(Param) && IsCompiled());
			return reinterpret_cast<T*>(CPUData.GetData() + Params->GetParams().Find(Param)->GetOffset());
		}

		const ubyte* GetDataPointer() const { return CPUData.GetData(); }
		usize GetDataSize() const { return CPUData.GetLength(); }
		inline EShaderStageFlags GetShaderStages() const { return Params->GetShaderStages(); }

	private:
		TRef<TPipelineParamMapping> Params;
		TDynArray<ubyte> CPUData;
		bool bIsCompiled = false;
	};
}