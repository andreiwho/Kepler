#pragma once
#include "Core/Core.h"
#include "Renderer/Elements/RenderTarget.h"
#include "Elements/Texture.h"


namespace Kepler
{
	class TRenderTargetGroup : public TRefCounted
	{
	public:
		TRenderTargetGroup(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers = 1);
		~TRenderTargetGroup();

		TRef<TRenderTarget2D> GetRenderTargetAtArrayLayer(u32 Index) const;
		TRef<TTextureSampler2D> GetTextureSamplerAtArrayLayer(u32 Index) const;

		void Resize(u32 InWidth, u32 InHeight, EFormat InFormat);

		static TRef<TRenderTargetGroup> New(u32 InWidth, u32 InHeight, EFormat InFormat, u32 InArrayLayers = 1);

	private:
		TDynArray<TRef<TRenderTarget2D>> RenderTargets;
		TDynArray<TRef<TTextureSampler2D>> TextureSamplers;
		u32 Width, Height;
		EFormat Format;
		u32 ArrayLayers;
	};
	
	class TTargetRegistry
	{
		static TTargetRegistry* Instance;
	public:
		TTargetRegistry();
		~TTargetRegistry();

		static TTargetRegistry* Get() { return Instance; }
		// Returns a render target with a specified name, size and format
		// - If size is different from the actual size, the render target is recreated and returned
		// - If no render target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		TRef<TRenderTargetGroup> GetRenderTargetGroup(const TString& Name, u32 Width = UINT32_MAX, u32 Height = UINT32_MAX, EFormat Format = EFormat::Unknown, u32 ArrayLayers = UINT32_MAX);

		// Returns a depth target with a specified name, size and format
		// - If size is different from the actual size, the depth target is recreated and returned
		// - If no depth target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		TRef<TDepthStencilTarget2D> GetDepthTarget(const TString& Name, u32 Width = UINT32_MAX, u32 Height = UINT32_MAX, EFormat Format = EFormat::Unknown, bool bSampled = false);

	private:
		TChaoticMap<TString, TRef<TRenderTargetGroup>> RenderTargets;
		TChaoticMap<TString, TRef<TDepthStencilTarget2D>> DepthTargets;
	};
}