#include "ParamPack.h"
#include <set>
#include "../LowLevelRenderer.h"

namespace ke
{
	TPipelineParamPack::TPipelineParamPack(RefPtr<PipelineParamMapping> Mapping)
		:	m_Params(Mapping)
	{
		struct TLocalParam
		{
			TString Name;
			PipelineParam Param;
		};

		Array<TLocalParam> SortedParams;
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
		const PipelineParam& LastParam = SortedParams[SortedParams.GetLength() - 1].Param;
		static constexpr auto FrameCount = LowLevelRenderer::m_SwapChainFrameCount;
		m_CPUData.Resize((LastParam.GetOffset() + LastParam.GetSize()) * FrameCount);	// Enable multi-buffering
		m_SinglePackStride = m_CPUData.GetLength() / FrameCount;
	}

	u8 TPipelineParamPack::GetBufferIndex() noexcept
	{
		return LowLevelRenderer::Get()->GetNextFrameIndex();
	}

	void PipelineParamMapping::AddParam(const TString& Name, usize Offset, usize Size, EShaderStageFlags Stage, EShaderInputType Type)
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
		m_Params.Insert(Name, PipelineParam(Offset, ActualSize, Type));
		m_ParamShaderStages |= Stage;
	}

	void PipelineParamMapping::AddTextureSampler(const TString& name, EShaderStageFlags stage, u32 reg)
	{
		CHECK(!name.empty());
		m_Samplers.Insert(name, reg);
		m_SamplerShaderStages |= stage;
	}

	RefPtr<TPipelineParamPack> PipelineParamMapping::CreateParamPack()
	{
		return MakeRef(ke::New<TPipelineParamPack>(RefFromThis()));
	}

	RefPtr<PipelineSamplerPack> PipelineParamMapping::CreateSamplerPack()
	{
		return MakeRef(ke::New<PipelineSamplerPack>(RefFromThis()));
	}

	//////////////////////////////////////////////////////////////////////////
	PipelineSamplerPack::PipelineSamplerPack(RefPtr<PipelineParamMapping> pMapping)
		:	m_Params(pMapping)
	{
		// Get register count and allocate space for the stuff
		u32 maxRegister = 0;
		for (const auto& [_, Register] : m_Params->GetSamplers())
		{
			if (Register > maxRegister)
			{
				maxRegister = Register;
			}
		}
		m_Samplers.Resize(1ull + maxRegister);
	}

	//////////////////////////////////////////////////////////////////////////
	void PipelineSamplerPack::Write(const TString& name, RefPtr<ITextureSampler2D> pData)
	{
		CHECK(m_Params);
		CHECK(m_Params->GetSamplers().Contains(name));
		const u32 reg = m_Params->GetSamplers()[name];
		m_Samplers[reg] = pData;
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ITextureSampler2D> PipelineSamplerPack::GetSampler(const TString& name)
	{
		CHECK(m_Params);
		CHECK(m_Params->GetSamplers().Contains(name));
		const u32 Register = m_Params->GetSamplers()[name];
		return m_Samplers[Register];
	}

}