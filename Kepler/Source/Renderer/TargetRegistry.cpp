#include "TargetRegistry.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET GROUP
	//////////////////////////////////////////////////////////////////////////
	TRenderTargetGroup::TRenderTargetGroup(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers)
		:	Width(0)
		,	Height(0)
		,	Format(InFormat)
		,	ArrayLayers(InArrayLayers)
	{
		RenderTargets.Resize(ArrayLayers);
		TextureSamplers.Resize(ArrayLayers);
		Resize(InWidth, InHeight, InFormat);
	}

	TRenderTargetGroup::~TRenderTargetGroup()
	{
		KEPLER_INFO(VALIDATE, "Log");
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTarget2D> TRenderTargetGroup::GetRenderTargetAtArrayLayer(u32 Index) const
	{
		CHECK(Index < ArrayLayers);
		return RenderTargets[Index];
	}

	TRef<TTextureSampler2D> TRenderTargetGroup::GetTextureSamplerAtArrayLayer(u32 Index) const
	{
		CHECK(Index < ArrayLayers);
		return TextureSamplers[Index];
	}

	//////////////////////////////////////////////////////////////////////////
	void TRenderTargetGroup::Resize(u32 InWidth, u32 InHeight, EFormat InFormat)
	{
		if (Width != InWidth || Height != InHeight || Format.Value != InFormat)
		{
			Width = InWidth;
			Height = InHeight;
			Format = InFormat;

			TRef<TImage2D> TargetImage = TImage2D::New(InWidth, InHeight, InFormat, EImageUsage::RenderTarget | EImageUsage::ShaderResource, 1, ArrayLayers);
			for (u32 Index = 0; Index < ArrayLayers; ++Index)
			{
				RenderTargets[Index] = TRenderTarget2D::New(TargetImage, 0, Index);
				TextureSamplers[Index] = TTextureSampler2D::New(TargetImage, 0, Index);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TRenderTargetGroup::New(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers)
	{
		return MakeRef(Kepler::New<TRenderTargetGroup>(InWidth, InHeight, InFormat, InArrayLayers));
	}

	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET REGISTRY
	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry* TTargetRegistry::Instance = nullptr;

	TTargetRegistry::TTargetRegistry()
	{
		Instance = this;
	}

	TTargetRegistry::~TTargetRegistry()
	{
		KEPLER_INFO(VALIDATE, "This is log");
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TTargetRegistry::GetRenderTargetGroup(const TString& Name, u32 Width, u32 Height, EFormat Format, u32 ArrayLayers)
	{
		if (!RenderTargets.Contains(Name))
		{
			CHECK(Width != UINT32_MAX && Height != UINT32_MAX && Format.Value != EFormat::Unknown && ArrayLayers > 0);
			RenderTargets[Name] = TRenderTargetGroup::New(Width, Height, Format, ArrayLayers);
		}

		// This may cause a performance problem, or may not. Needs to be checked
		TRef<TRenderTargetGroup> Target = RenderTargets[Name];
		if (Width != UINT32_MAX || Height != UINT32_MAX || Format != EFormat::Unknown)
		{
			Target->Resize(Width, Height, Format);
		}
		return Target;
	}

	TRef<TDepthStencilTarget2D> TTargetRegistry::GetDepthTarget(const TString& Name, u32 Width, u32 Height, EFormat Format, bool bSampled)
	{
		// Create
		if (!DepthTargets.Contains(Name))
		{
			CHECK(Width != UINT32_MAX && Height != UINT32_MAX && Format.Value != EFormat::Unknown);
			EImageUsage Flags = EImageUsage::DepthTarget;
			if (bSampled)
			{
				Flags.Mask |= EImageUsage::ShaderResource;
			}

			TRef<TImage2D> DepthImage = TImage2D::New(Width, Height, Format, Flags);
			DepthTargets[Name] = TDepthStencilTarget2D::New(DepthImage);
		}

		// Acquire | Resize
		TRef<TDepthStencilTarget2D> DepthTarget = DepthTargets[Name];
		if (Width != UINT32_MAX || Height != UINT32_MAX)
		{
			if (DepthTarget->GetWidth() != Width || DepthTarget->GetHeight() != Height)
			{
				TRef<TImage2D> DepthImage = TImage2D::New(Width, Height, DepthTarget->GetFormat(), DepthTarget->GetImage()->GetUsage());
				DepthTarget = TDepthStencilTarget2D::New(DepthImage);
				DepthTargets[Name] = DepthTarget;
			}
		}
		return DepthTarget;
	}

}