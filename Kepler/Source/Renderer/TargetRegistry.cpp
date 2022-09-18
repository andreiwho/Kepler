#include "TargetRegistry.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET GROUP
	//////////////////////////////////////////////////////////////////////////
	TRenderTargetGroup::TRenderTargetGroup(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers, bool bAllowCPURead)
		:	Width(0)
		,	Height(0)
		,	Format(InFormat)
		,	ArrayLayers(InArrayLayers)
	{
		RenderTargets.Resize(ArrayLayers);
		if (!bAllowCPURead)
		{
			TextureSamplers.Resize(ArrayLayers);
		}
		Resize(InWidth, InHeight, InFormat, bAllowCPURead);
	}

	//////////////////////////////////////////////////////////////////////////
	TRenderTargetGroup::~TRenderTargetGroup()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTarget2D> TRenderTargetGroup::GetRenderTargetAtArrayLayer(u32 Index) const
	{
		CHECK(Index < ArrayLayers);
		return RenderTargets[Index];
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TTextureSampler2D> TRenderTargetGroup::GetTextureSamplerAtArrayLayer(u32 Index) const
	{
		CHECK(Index < ArrayLayers);
		return TextureSamplers[Index];
	}

	//////////////////////////////////////////////////////////////////////////
	void TRenderTargetGroup::Resize(u32 InWidth, u32 InHeight, EFormat InFormat, bool bAllowCPURead)
	{
		if (Width != InWidth || Height != InHeight || Format.Value != InFormat)
		{
			Width = InWidth > 0 ? InWidth : 1;
			Height = InHeight > 0 ? InHeight : 1;
			Format = InFormat;

			TRef<TImage2D> TargetImage = TImage2D::New(Width, Height, InFormat, 
				EImageUsage::RenderTarget | (bAllowCPURead ? EImageUsage::AllowCPURead : EImageUsage::ShaderResource), 
				1, 
				ArrayLayers);
			for (u32 Index = 0; Index < ArrayLayers; ++Index)
			{
				RenderTargets[Index] = TRenderTarget2D::New(TargetImage, 0, Index);
				if (!bAllowCPURead)
				{
					TextureSamplers[Index] = TTextureSampler2D::New(TargetImage, 0, Index);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TRenderTargetGroup::New(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers, bool bAllowCPURead)
	{
		return MakeRef(Kepler::New<TRenderTargetGroup>(InWidth, InHeight, InFormat, InArrayLayers));
	}

	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET REGISTRY
	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry* TTargetRegistry::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry::TTargetRegistry()
	{
		Instance = this;
	}

	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry::~TTargetRegistry()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TTargetRegistry::GetRenderTargetGroup(const TString& Name, u32 Width, u32 Height, EFormat Format, u32 ArrayLayers, bool bAllowCPURead)
	{
		KEPLER_PROFILE_SCOPE();
		if (!RenderTargets.Contains(Name))
		{
			CHECK(Width != UINT32_MAX && Height != UINT32_MAX && Format.Value != EFormat::Unknown && ArrayLayers > 0);
			RenderTargets[Name] = TRenderTargetGroup::New(Width, Height, Format, ArrayLayers);
		}

		// This may cause a performance problem, or may not. Needs to be checked
		TRef<TRenderTargetGroup> Target = RenderTargets[Name];
		if (Width != UINT32_MAX || Height != UINT32_MAX || Format != EFormat::Unknown)
		{
			Target->Resize(Width, Height, Format, bAllowCPURead);
		}
		return Target;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TTargetRegistry::RenderTargetGroupExists(const TString& Name) const
	{
		return RenderTargets.Contains(Name);
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TDepthStencilTarget2D> TTargetRegistry::GetDepthTarget(const TString& Name, u32 Width, u32 Height, EFormat Format, bool bSampled)
	{
		// Create
		auto NewWidth = Width > 0 ? Width : 1;
		auto NewHeight = Height > 0 ? Height : 1;
		if (!DepthTargets.Contains(Name))
		{
			CHECK(Width != UINT32_MAX && Height != UINT32_MAX && Format.Value != EFormat::Unknown);
			EImageUsage Flags = EImageUsage::DepthTarget;
			if (bSampled)
			{
				Flags.Mask |= EImageUsage::ShaderResource;
			}

			TRef<TImage2D> DepthImage = TImage2D::New(NewWidth, NewHeight, Format, Flags);
			DepthTargets[Name] = TDepthStencilTarget2D::New(DepthImage);
		}

		// Acquire | Resize
		TRef<TDepthStencilTarget2D> DepthTarget = DepthTargets[Name];
		if (NewWidth != UINT32_MAX || NewHeight != UINT32_MAX)
		{
			if (DepthTarget->GetWidth() != NewWidth || DepthTarget->GetHeight() != NewHeight)
			{
				TRef<TImage2D> DepthImage = TImage2D::New(NewWidth, NewHeight, DepthTarget->GetFormat(), DepthTarget->GetImage()->GetUsage());
				DepthTarget = TDepthStencilTarget2D::New(DepthImage);
				DepthTargets[Name] = DepthTarget;
			}
		}
		return DepthTarget;
	}

}