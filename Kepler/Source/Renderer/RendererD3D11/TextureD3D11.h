#pragma once
#include "Renderer/Elements/Texture.h"
#include "D3D11Common.h"

namespace ke
{
	class TTextureSampler2D_D3D11 : public ITextureSampler2D
	{
	public:
		TTextureSampler2D_D3D11(RefPtr<IImage2D> InImage, u32 MipLevel, u32 ArrayLayer);
		~TTextureSampler2D_D3D11();

		inline ID3D11ShaderResourceView* GetView() const { return View; }
		inline ID3D11SamplerState* GetSampler() const { return Sampler; }
		virtual void* GetNativeHandle() const { return GetView(); };

	private:
		ID3D11ShaderResourceView* View{};
		ID3D11SamplerState* Sampler{};
	};
}