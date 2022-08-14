#pragma once
#ifdef WIN32
#include "D3D11Common.h"
#include "Renderer/Elements/SwapChain.h"

namespace Kepler
{
	class TSwapChainD3D11 : public TSwapChain
	{
	public:
		TSwapChainD3D11(class TWindow* Window);
		~TSwapChainD3D11();

	protected:
		void Present() override;
		void Resize(i32 Width, i32 Heigt) override;
		virtual void ReleaseResources() override;

	private:
		IDXGISwapChain4* SwapChain{};
	};
}
#endif