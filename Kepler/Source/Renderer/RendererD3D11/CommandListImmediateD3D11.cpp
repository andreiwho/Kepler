#include "CommandListImmediateD3D11.h"
#include "SwapChainD3D11.h"
#include "Renderer/RenderGlobals.h"

namespace Kepler
{
	TCommandListImmediateD3D11* TCommandListImmediateD3D11::Instance = nullptr;

	TCommandListImmediateD3D11::TCommandListImmediateD3D11(ID3D11DeviceContext4* InContext) : Context(InContext)
	{
		CHECK(!Instance);
		Instance = this;

		CHECK(IsRenderThread());
		Context->AddRef();
	}

	TCommandListImmediateD3D11::~TCommandListImmediateD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
		Context->Release();
	}

	void TCommandListImmediateD3D11::StartDrawingToSwapChainImage(TSwapChain* SwapChain)
	{
		CHECK(IsRenderThread());
		CHECK(SwapChain && Context);
		TSwapChainD3D11* MySwapChain = static_cast<TSwapChainD3D11*>(SwapChain);
		ID3D11RenderTargetView* ppRTV[] = { CHECKED(MySwapChain->GetRenderTargetView()) };
		Context->OMSetRenderTargets(ARRAYSIZE(ppRTV), ppRTV, nullptr);
	}

	void TCommandListImmediateD3D11::ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4])
	{
		CHECK(IsRenderThread());
		CHECK(SwapChain && Context);
		TSwapChainD3D11* MySwapChain = static_cast<TSwapChainD3D11*>(SwapChain);
		Context->ClearRenderTargetView(MySwapChain->GetRenderTargetView(), ClearColor);
	}

	void TCommandListImmediateD3D11::Draw(u32 VertexCount, u32 BaseVertexIndex)
	{
		CHECK(IsRenderThread());
		CHECK(Context);
		Context->Draw(VertexCount, BaseVertexIndex);
	}

}