#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace Kepler
{
	class TTextureSampler2D : public TEnableRefFromThis<TTextureSampler2D>
	{
	protected:
		TTextureSampler2D(TRef<TImage2D> InImage) : Image(InImage) {}

	public:
		static TRef<TTextureSampler2D> New(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);

		inline TRef<TImage2D> GetImage() const { return Image; }

	protected:
		TRef<TImage2D> Image;
	};
}