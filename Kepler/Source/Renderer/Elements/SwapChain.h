#pragma once
#include "Core/Types.h"

namespace Kepler
{
	class TSwapChain
	{
	public:
		TSwapChain(class TWindow* Window);
		virtual ~TSwapChain();

		void RT_Present();
		void RT_Resize(i32 Width, i32 Height);

	protected:
		TWindow* MyWindow{};

		i32 Width{}, Height{};
		i32 ImageCount{3};

		virtual void Present() = 0;
		virtual void Resize(i32 Width, i32 Heigt) = 0;
		virtual void ReleaseResources() = 0;
	};
}