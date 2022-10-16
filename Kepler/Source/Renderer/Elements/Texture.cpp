#include "Texture.h"
#include "../RenderGlobals.h"
#include "../RenderDevice.h"

namespace ke
{

	RefPtr<ITextureSampler2D> ITextureSampler2D::New(RefPtr<IImage2D> pImage, u32 mipLevel, u32 arrayLayer)
	{
		return GetRenderDevice()->CreateTextureSampler2D(pImage, mipLevel, arrayLayer);
	}

}