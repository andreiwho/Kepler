#include "ParamPack.h"
#include <set>

namespace ke
{
	TPipelineParamPack::TPipelineParamPack(TRef<TPipelineParamMapping> Mapping)
		:	Params(Mapping)
	{
		struct TLocalParam
		{
			TString Name;
			TPipelineParam Param;
		};

		TDynArray<TLocalParam> SortedParams;
		SortedParams.Reserve(Mapping->GetParams().GetLength());

		for (auto& [Name, Value] : Mapping->GetParams())
		{
			SortedParams.EmplaceBack(TLocalParam{ Name, Value });
		}
		SortedParams.Sort(
			[](const TLocalParam& Lhs, const TLocalParam& Rhs)
			{
				return Lhs.Param.GetOffset() < Rhs.Param.GetOffset();
			});

		// Check if params are overlapping
#if ENABLE_DEBUG
		// TODO
#endif

		// Allocate space
		const TPipelineParam& LastParam = SortedParams[SortedParams.GetLength() - 1].Param;
		CPUData.Resize(LastParam.GetOffset() + LastParam.GetSize());
		bIsCompiled = true;
	}

	void TPipelineParamMapping::AddParam(const TString& Name, usize Offset, usize Size, EShaderStageFlags Stage, EShaderInputType Type)
	{
		CHECK(!Name.empty());

		usize ActualSize = Size;
		if (Size == 0 && Type != EShaderInputType::Custom)
		{
			ActualSize = Type.GetValueSize();
		}
		else if (Type == EShaderInputType::Custom)
		{
			CHECKMSG(Size > 0, "You must specify size for EShaderInputType::Custom");
		}

		CHECK(ActualSize > 0);
		Params.Insert(Name, TPipelineParam(Offset, ActualSize, Type));
		ParamShaderStages |= Stage;
	}

	void TPipelineParamMapping::AddTextureSampler(const TString& Name, EShaderStageFlags Stage, u32 Register)
	{
		CHECK(!Name.empty());
		Samplers.Insert(Name, Register);
		SamplerShaderStages |= Stage;
	}

	TRef<TPipelineParamPack> TPipelineParamMapping::CreateParamPack()
	{
		return MakeRef(ke::New<TPipelineParamPack>(RefFromThis()));
	}

	TRef<TPipelineSamplerPack> TPipelineParamMapping::CreateSamplerPack()
	{
		return MakeRef(ke::New<TPipelineSamplerPack>(RefFromThis()));
	}

	//////////////////////////////////////////////////////////////////////////
	TPipelineSamplerPack::TPipelineSamplerPack(TRef<TPipelineParamMapping> Mapping)
		:	Params(Mapping)
	{
		// Get register count and allocate space for the stuff
		u32 MaxRegister = 0;
		for (const auto& [_, Register] : Params->GetSamplers())
		{
			if (Register > MaxRegister)
			{
				MaxRegister = Register;
			}
		}
		Samplers.Resize(1ull + MaxRegister);
	}

	//////////////////////////////////////////////////////////////////////////
	void TPipelineSamplerPack::Write(const TString& Name, TRef<TTextureSampler2D> Data)
	{
		CHECK(Params);
		CHECK(Params->GetSamplers().Contains(Name));
		const u32 Register = Params->GetSamplers()[Name];
		Samplers[Register] = Data;
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TTextureSampler2D> TPipelineSamplerPack::GetSampler(const TString& Name)
	{
		CHECK(Params);
		CHECK(Params->GetSamplers().Contains(Name));
		const u32 Register = Params->GetSamplers()[Name];
		return Samplers[Register];
	}

}