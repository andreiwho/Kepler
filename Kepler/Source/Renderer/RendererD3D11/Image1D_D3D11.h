#pragma once
#include "Renderer/Elements/Image.h"
#include "D3D11Common.h"

namespace Kepler
{
	class TImage1D_D3D11 : public TImage1D
	{
	public:
		TImage1D_D3D11(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage1D_D3D11();
	
	private:
		ID3D11Texture1D* Image{};
	};
}