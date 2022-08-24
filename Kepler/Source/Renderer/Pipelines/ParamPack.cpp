#include "ParamPack.h"
#include <set>

namespace Kepler
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
		ShaderStages |= Stage;
	}

	TRef<TPipelineParamPack> TPipelineParamMapping::CreatePack()
	{
		return MakeRef(New<TPipelineParamPack>(RefFromThis()));
	}

}