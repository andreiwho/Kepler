#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class RenderTarget2D : public TEnableRefFromThis<RenderTarget2D>
	{
	protected:
		RenderTarget2D(TRef<TImage2D> InImage, u32 MipLevl = 0, u32 ArrayLayer = 0);
	
	public:
		TRef<TImage2D> GetImage() const { return m_Image; }
		inline u32 GetWidth() const { return m_Image->GetWidth(); }
		inline u32 GetHeight() const { return m_Image->GetHeight(); }
		inline EFormat GetFormat() const { return m_Image->GetFormat(); }
		virtual void* GetNativeHandle() const = 0;

		static TRef<RenderTarget2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
	
	protected:
		TRef<TImage2D> m_Image{};
	};

	//////////////////////////////////////////////////////////////////////////
	class DepthStencilTarget2D : public TEnableRefFromThis<DepthStencilTarget2D>
	{
	protected:
		DepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		
	public:
		TRef<TImage2D> GetImage() const { return m_Image; }
		inline u32 GetWidth() const { return m_Image->GetWidth(); }
		inline u32 GetHeight() const { return m_Image->GetHeight(); }
		inline EFormat GetFormat() const { return m_Image->GetFormat(); }

		static TRef<DepthStencilTarget2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);

	protected:
		TRef<TImage2D> m_Image{};
	};
}