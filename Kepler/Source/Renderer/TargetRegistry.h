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

		RefPtr<IRenderTarget2D> GetRenderTargetAtArrayLayer(u32 idx) const;
		RefPtr<ITextureSampler2D> GetTextureSamplerAtArrayLayer(u32 idx) const;

		void Resize(u32 width, u32 height, EFormat format, bool bAllowCPURead);

		static RefPtr<TRenderTargetGroup> New(u32 width, u32 height, EFormat format, u32 layers = 1, bool bAllowCPURead = false);

	private:
		Array<RefPtr<IRenderTarget2D>> m_RenderTargets;
		Array<RefPtr<ITextureSampler2D>> m_TextureSamplers;
		u32 m_Width;
		u32 m_Height;
		EFormat m_Format;
		u32 m_ArrayLayers;
	};
	
	class RenderTargetRegistry
	{
		static RenderTargetRegistry* Instance;
	public:
		RenderTargetRegistry();
		~RenderTargetRegistry();

		static RenderTargetRegistry* Get() { return Instance; }
		// Returns a render target with a specified name, size and format
		// - If size is different from the actual size, the render target is recreated and returned
		// - If no render target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		RefPtr<TRenderTargetGroup> GetRenderTargetGroup(const String& name, u32 width = UINT32_MAX, u32 height = UINT32_MAX, EFormat format = EFormat::Unknown, u32 layers = UINT32_MAX, bool bAllowCPURead = false);

		bool RenderTargetGroupExists(const String& name) const;

		// Returns a depth target with a specified name, size and format
		// - If size is different from the actual size, the depth target is recreated and returned
		// - If no depth target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		RefPtr<IDepthStencilTarget2D> GetDepthTarget(const String& name, u32 width = UINT32_MAX, u32 height = UINT32_MAX, EFormat format = EFormat::Unknown, bool bSampled = false);

		// Returns a depth target with a specified name, size and format
		// - If size is different from the actual size, the depth target is recreated and returned
		// - If no depth target exists with this name, the new one gets created
		// Name is required, Width, Height, Format are important only for the first time.
		RefPtr<IDepthStencilTarget2D> GetReadOnlyDepthTarget(const String& name);

	private:
		Map<String, RefPtr<TRenderTargetGroup>> m_RenderTargets;
		Map<String, RefPtr<IDepthStencilTarget2D>> m_DepthTargets;
		Map<String, RefPtr<IDepthStencilTarget2D>> m_ReadOnlyDepthTargets;
	};
}