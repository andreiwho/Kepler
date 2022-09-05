#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	class TRenderTarget2D : public TEnableRefFromThis<TRenderTarget2D>
	{
	protected:
		TRenderTarget2D(TRef<TImage2D> InImage, u32 MipLevl = 0, u32 ArrayLayer = 0);
	
	public:
		TRef<TImage2D> GetImage() const { return Image; }
		inline u32 GetWidth() const { return Image->GetWidth(); }
		inline u32 GetHeight() const { return Image->GetHeight(); }
		inline EFormat GetFormat() const { return Image->GetFormat(); }
		virtual void* GetNativeHandle() const = 0;

		static TRef<TRenderTarget2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
	
	protected:
		TRef<TImage2D> Image{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TDepthStencilTarget2D : public TEnableRefFromThis<TDepthStencilTarget2D>
	{
	protected:
		TDepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		
	public:
		TRef<TImage2D> GetImage() const { return Image; }
		inline u32 GetWidth() const { return Image->GetWidth(); }
		inline u32 GetHeight() const { return Image->GetHeight(); }
		inline EFormat GetFormat() const { return Image->GetFormat(); }

		static TRef<TDepthStencilTarget2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);

	protected:
		TRef<TImage2D> Image{};
	};
}