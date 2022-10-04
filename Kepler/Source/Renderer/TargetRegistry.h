#pragma once
#include "Core/Core.h"
#include "Renderer/Elements/RenderTarget.h"
#include "Elements/Texture.h"


namespace ke
{
	class TRenderTargetGroup : public IntrusiveRefCounted
	{
	public:
		TRenderTargetGroup(u32 width, u32 height, EFormat format, u32 layers = 1, bool bAllowCPURead = false);
		~TRenderTargetGroup();

		TRef<RenderTarget2D> GetRenderTargetAtArrayLayer(u32 idx) const;
		TRef<TTextureSampler2D> GetTextureSamplerAtArrayLayer(u32 idx) const;

		void Resize(u32 width, u32 height, EFormat format, bool bAllowCPURead);

		static TRef<TRenderTargetGroup> New(u32 width, u32 height, EFormat format, u32 layers = 1, bool bAllowCPURead = false);

	private:
		Array<TRef<RenderTarget2D>> m_RenderTargets;
		Array<TRef<TTextureSampler2D>> m_TextureSamplers;
		u32 m_Width;
		u32 m_Height;
		EFormat m_Format;
		u32 m_ArrayLayers;
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
		TRef<TRenderTargetGroup> GetRenderTargetGroup(const TString& name, u32 width = UINT32_MAX, u32 height = UINT32_MAX, EFormat format = EFormat::Unknown, u32 layers = UINT32_MAX, bool bAllowCPURead = false);

		bool RenderTargetGroupExists(const TString& name) const;

		// Returns a depth target with a specified name, size and format
		// - If size is different from the actual size, the depth target is recreated and returned
		// - If no depth target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		TRef<DepthStencilTarget2D> GetDepthTarget(const TString& name, u32 width = UINT32_MAX, u32 height = UINT32_MAX, EFormat format = EFormat::Unknown, bool bSampled = false);

	private:
		Map<TString, TRef<TRenderTargetGroup>> m_RenderTargets;
		Map<TString, TRef<DepthStencilTarget2D>> m_DepthTargets;
	};
}