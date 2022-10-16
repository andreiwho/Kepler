#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class IRenderTarget2D : public EnableRefPtrFromThis<IRenderTarget2D>
	{
	protected:
		IRenderTarget2D(RefPtr<IImage2D> image, u32 mipLevel = 0, u32 arrayLayer = 0);
	
	public:
		RefPtr<IImage2D> GetImage() const { return m_Image; }
		inline u32 GetWidth() const { return m_Image->GetWidth(); }
		inline u32 GetHeight() const { return m_Image->GetHeight(); }
		inline EFormat GetFormat() const { return m_Image->GetFormat(); }
		virtual void* GetNativeHandle() const = 0;

		static RefPtr<IRenderTarget2D> New(RefPtr<IImage2D> image, u32 mipLevel = 0, u32 arrayLayer = 0);
	
	protected:
		RefPtr<IImage2D> m_Image{};
	};

	//////////////////////////////////////////////////////////////////////////
	class IDepthStencilTarget2D : public EnableRefPtrFromThis<IDepthStencilTarget2D>
	{
	protected:
		IDepthStencilTarget2D(RefPtr<IImage2D> pImage, u32 mipLevel = 0, u32 arrayLayer = 0, bool bReadOnly = false);
		
	public:
		RefPtr<IImage2D> GetImage() const { return m_Image; }
		inline u32 GetWidth() const { return m_Image->GetWidth(); }
		inline u32 GetHeight() const { return m_Image->GetHeight(); }
		inline EFormat GetFormat() const { return m_Image->GetFormat(); }

		static RefPtr<IDepthStencilTarget2D> New(RefPtr<IImage2D> pImage, u32 mipLevel = 0, u32 arrayLayer = 0);
		static RefPtr<IDepthStencilTarget2D> NewReadOnly(RefPtr<IImage2D> pImage, u32 mipLevel = 0, u32 arrayLayer = 0);

	protected:
		RefPtr<IImage2D> m_Image{};
	};
}