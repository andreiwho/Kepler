#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
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

	class TPipelineParamPack : public TRefCounted
	{
	public:
		void AddParam(const TString& Name, usize Offset, usize Size, EShaderInputType Type = EShaderInputType::Custom);
		void Compile();

		inline bool IsCompiled() const { return bIsCompiled; }
		inline usize GetSizeInBytes() const { return CPUData.GetLength(); }

		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			CHECK(Params.Contains(Param) && IsCompiled());

			const auto& ParamRef = Params[Param];
			memcpy(CPUData.GetData() + ParamRef.GetOffset(), Data, ParamRef.GetSize());
		}
		
		template<typename T>
		T* GetParam(const TString& Param)
		{
			CHECK(Params.Contains(Param) && IsCompiled());
			return reinterpret_cast<T*>(CPUData.GetData() + Params.Find(Param)->GetOffset());
		}

	private:
		TChaoticMap<TString, TPipelineParam> Params;
		TDynArray<ubyte> CPUData;
		bool bIsCompiled = false;
	};

#define ADD_PIPELINE_PARAM(Pack, Struct, Name, Type) (Pack).AddParam(#Name, offsetof(Struct, Name), sizeof(Struct::Name), Type)
}