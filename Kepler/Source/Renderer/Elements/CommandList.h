#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

namespace Kepler
{
	class TSwapChain;

	class TCommandList
	{
	public:
		virtual ~TCommandList() = default;

		virtual void StartDrawingToSwapChainImage(TSwapChain* SwapChain) = 0;
		virtual void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]) = 0;
	};

	class TCommandListImmediate : public TCommandList
	{
	};

	class TImmediateCommandListProxy
	{
	public:
		TImmediateCommandListProxy(TRef<TCommandListImmediate> List) : CommandList(List) {}

		void StartDrawingToSwapChainImage(TSwapChain* SwapChain);
		void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]);

	private:
		TRef<TCommandListImmediate> CommandList;
	};
}