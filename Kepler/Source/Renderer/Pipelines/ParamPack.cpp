#include "ParamPack.h"
#include "Math/Vector.h"
#include <set>

namespace Kepler
{
	void TPipelineParamPack::AddParam(const TString& Name, usize Offset, usize Size, EShaderInputType Type)
	{
		CHECK(!bIsCompiled);
		CHECK(!Name.empty());

		Params.Insert(Name, TPipelineParam(Offset, Size, Type));
	}

	void TPipelineParamPack::Compile()
	{
		struct TLocalParam
		{
			TString Name;
			TPipelineParam Param;
		};

		TDynArray<TLocalParam> SortedParams;
		SortedParams.Reserve(Params.GetLength());

		for (auto& [Name, Value] : Params)
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
}