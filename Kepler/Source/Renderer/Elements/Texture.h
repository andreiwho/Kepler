#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace ke
{
	class TTextureSampler2D : public TEnableRefFromThis<TTextureSampler2D>
	{
	protected:
		TTextureSampler2D(TRef<TImage2D> InImage) : Image(InImage) {}

	public:
		static TRef<TTextureSampler2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);

		inline TRef<TImage2D> GetImage() const { return Image; }

		virtual void* GetNativeHandle() const = 0;
	protected:
		TRef<TImage2D> Image;
	};
}