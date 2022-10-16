#include "TextureD3D11.h"
#include "Renderer/RenderGlobals.h"
#include "ImageD3D11.h"
#include "RenderDeviceD3D11.h"

namespace ke
{
	TTextureSampler2D_D3D11::TTextureSampler2D_D3D11(RefPtr<IImage2D> InImage, u32 MipLevel, u32 ArrayLayer)
		: ITextureSampler2D(InImage)
	{
		CHECK(IsRenderThread());
		CHECK(InImage);

		if (auto MyImage = RefCast<TImage2D_D3D11>(InImage))
		{
			CD3D11_SHADER_RESOURCE_VIEW_DESC Desc(
				MyImage->GetImage(),
				D3D11_SRV_DIMENSION_TEXTURE2D,
				(DXGI_FORMAT)MyImage->GetFormat().Value,
				MipLevel,
				1,
				ArrayLayer,
				1
			);
			if (auto Device = TRenderDeviceD3D11::Get()->GetDevice())
			{
				CD3D11_SAMPLER_DESC SamplerDesc(D3D11_DEFAULT);

				// TODO: Setup the sampler
				HRCHECK(Device->CreateSamplerState(&SamplerDesc, &Sampler));
				HRCHECK(Device->CreateShaderResourceView(MyImage->GetImage(), &Desc, &View));
			}
		}
	}

	TTextureSampler2D_D3D11::~TTextureSampler2D_D3D11()
	{
		if (!View)
		{
			return;
		}

		if (auto Device = TRenderDeviceD3D11::Get())
		{
			if (View)
				Device->RegisterPendingDeleteResource(View);
			if (Sampler)
				Device->RegisterPendingDeleteResource(Sampler);
		}
	}
}