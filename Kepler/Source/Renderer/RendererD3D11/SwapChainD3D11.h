#pragma once
#ifdef WIN32
#include "D3D11Common.h"
#include "Renderer/Elements/SwapChain.h"

namespace ke
{
	class TSwapChainD3D11 : public TSwapChain
	{
	public:
		TSwapChainD3D11(class TWindow* Window);
		~TSwapChainD3D11();

		inline ID3D11RenderTargetView1* GetRenderTargetView() const { return RenderTargetView; }
	private:
		void CreateRenderTargets();

	protected:
		void Present() override;
		void Resize(i32 Width, i32 Heigt) override;

	private:
		IDXGISwapChain4* SwapChain{};
		ID3D11RenderTargetView1* RenderTargetView{};
	};
}
#endif