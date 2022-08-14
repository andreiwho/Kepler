#pragma once
#include "Core/Types.h"

namespace Kepler
{
	class TSwapChain
	{
	public:
		TSwapChain(class TWindow* Window);
		virtual ~TSwapChain() = default;

		virtual void Present() = 0;
		virtual void Resize(i32 Width, i32 Height) = 0;
		
		inline TWindow* GetAssociatedWindow() const { return MyWindow; }

	protected:
		TWindow* MyWindow{};

		i32 Width{}, Height{};
		i32 ImageCount{3};
	};
}