#include "Texture.h"
#include "../RenderGlobals.h"
#include "../RenderDevice.h"

namespace Kepler
{

	TRef<TTextureSampler2D> TTextureSampler2D::New(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
	{
		return GetRenderDevice()->CreateTextureSampler2D(InImage, MipLevel, ArrayLayer);
	}

}