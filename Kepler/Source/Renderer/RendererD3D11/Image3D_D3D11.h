#pragma once
#include "Renderer/Elements/Image.h"
#include "D3D11Common.h"

namespace Kepler
{
	class TImage3D_D3D11 : public TImage3D
	{
	public:
		TImage3D_D3D11(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage3D_D3D11();

	private:
		ID3D11Texture3D* Image{};
	};
}