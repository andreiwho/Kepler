#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace ke
{
	class TTextureSampler2D : public TEnableRefFromThis<TTextureSampler2D>
	{
	protected:
		TTextureSampler2D(RefPtr<TImage2D> InImage) : Image(InImage) {}

	public:
		static RefPtr<TTextureSampler2D> New(RefPtr<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);

		inline RefPtr<TImage2D> GetImage() const { return Image; }

		virtual void* GetNativeHandle() const = 0;
	protected:
		RefPtr<TImage2D> Image;
	};
}