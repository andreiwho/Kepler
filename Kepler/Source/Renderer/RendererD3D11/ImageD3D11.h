#pragma once
#include "Renderer/Elements/Image.h"
#include "D3D11Common.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class TImage2D_D3D11 : public TImage2D
	{
	public:
		TImage2D_D3D11(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage2D_D3D11();

		inline auto GetImage() const { return Image; }
		inline auto GetReadbackImage() const { return ReadbackImage; }
		void RequireReadbackCopy(RefPtr<class GraphicsCommandListImmediate> pImmCtx);

	private:
		ID3D11Texture2D* Image{};
		ID3D11Texture2D* ReadbackImage{};
	};
}