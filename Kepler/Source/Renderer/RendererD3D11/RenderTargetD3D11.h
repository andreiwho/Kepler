#pragma once
#include "Renderer/Elements/RenderTarget.h"
#include "D3D11Common.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	class TRenderTarget2D_D3D11 : public TRenderTarget2D
	{
	public:
		TRenderTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		~TRenderTarget2D_D3D11();

		inline ID3D11RenderTargetView* GetView() const { return RenderTarget; }

	private:
		ID3D11RenderTargetView* RenderTarget{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TDepthStencilTarget2D_D3D11 : public TDepthStencilTarget2D
	{
	public:
		TDepthStencilTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		~TDepthStencilTarget2D_D3D11();

		inline ID3D11DepthStencilView* GetView() const { return View; }

	private:
		ID3D11DepthStencilView* View{};
	};
}