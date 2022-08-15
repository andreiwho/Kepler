#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

namespace Kepler
{
	class TSwapChain;

	class TCommandList : public TRefCounted
	{
	public:
		virtual ~TCommandList() = default;

		virtual void StartDrawingToSwapChainImage(TSwapChain* SwapChain) = 0;
		virtual void ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4]) = 0;
		virtual void Draw(u32 VertexCount, u32 BaseVertexIndex) = 0;
	};

	class TCommandListImmediate : public TCommandList
	{
	};
}