#pragma once
#include "Renderer/Elements/Texture.h"
#include "D3D11Common.h"

namespace Kepler
{
	class TTextureSampler2D_D3D11 : public TTextureSampler2D
	{
	public:
		TTextureSampler2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel, u32 ArrayLayer);
		~TTextureSampler2D_D3D11();

		inline ID3D11ShaderResourceView* GetView() const { return View; }
		inline ID3D11SamplerState* GetSampler() const { return Sampler; }

	private:
		ID3D11ShaderResourceView* View{};
		ID3D11SamplerState* Sampler{};
	};
}