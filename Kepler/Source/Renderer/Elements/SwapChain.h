#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

namespace ke
{
	class ISwapChain : public IntrusiveRefCounted
	{
	public:
		ISwapChain(class TWindow* pWindow);
		virtual ~ISwapChain() = default;

		virtual void Present(bool bWaitForVerticalBlank) = 0;
		virtual void Resize(i32 width, i32 height) = 0;
		
		inline TWindow* GetAssociatedWindow() const { return m_Window; }

	protected:
		TWindow* m_Window{};

		i32 m_Width{}, m_Height{};
		i32 m_ImageCount{3};
	};
}