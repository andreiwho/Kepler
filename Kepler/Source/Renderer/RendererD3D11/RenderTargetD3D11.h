#pragma once
#include "Renderer/Elements/RenderTarget.h"
#include "D3D11Common.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class RenderTarget2D_D3D11 : public RenderTarget2D
	{
	public:
		RenderTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		~RenderTarget2D_D3D11();

		inline ID3D11RenderTargetView* GetView() const { return m_RenderTarget; }
		virtual void* GetNativeHandle() const override { return GetView(); }
	private:
		ID3D11RenderTargetView* m_RenderTarget{};
	};

	//////////////////////////////////////////////////////////////////////////
	class DepthStencilTarget2D_D3D11 : public DepthStencilTarget2D
	{
	public:
		DepthStencilTarget2D_D3D11(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0);
		~DepthStencilTarget2D_D3D11();

		inline ID3D11DepthStencilView* GetView() const { return m_View; }

	private:
		ID3D11DepthStencilView* m_View{};
	};
}