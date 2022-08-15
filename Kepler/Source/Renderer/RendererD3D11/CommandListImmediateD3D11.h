#pragma once
#include "D3D11Common.h"
#include "Renderer/Elements/CommandList.h"

namespace Kepler
{
	class TSwapChain;

	class TCommandListImmediateD3D11 : public TCommandListImmediate
	{
		static TCommandListImmediateD3D11* Instance;

	public:
		TCommandListImmediateD3D11(ID3D11DeviceContext4* InContext);
		~TCommandListImmediateD3D11();

		virtual void StartDrawingToSwapChainImage(TSwapChain* SwapChain) override;
		virtual void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]) override;

		static TCommandListImmediateD3D11* Get() { return CHECKED(Instance); }
	private:
		ID3D11DeviceContext4* Context{};
	};
}