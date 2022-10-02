#pragma once
#include "Renderer/Elements/Image.h"
#include "D3D11Common.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class TImage1D_D3D11 : public TImage1D
	{
	public:
		TImage1D_D3D11(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage1D_D3D11();

		inline auto GetImage() const { return Image; }

	private:
		ID3D11Texture1D* Image{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TImage2D_D3D11 : public TImage2D
	{
	public:
		TImage2D_D3D11(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage2D_D3D11();

		inline auto GetImage() const { return Image; }
		inline auto GetReadbackImage() const { return ReadbackImage; }
		void RequireReadbackCopy(TRef<class TCommandListImmediate> pImmCtx);

	private:
		ID3D11Texture2D* Image{};
		ID3D11Texture2D* ReadbackImage{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TImage3D_D3D11 : public TImage3D
	{
	public:
		TImage3D_D3D11(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);
		~TImage3D_D3D11();

		inline auto GetImage() const { return Image; }
	
	private:
		ID3D11Texture3D* Image{};
	};
}