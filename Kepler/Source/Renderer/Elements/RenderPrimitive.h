#pragma once
#include "Core/Types.h"

#include <atomic>

namespace ke
{
	class IRenderPrimitive
	{
	public:
		IRenderPrimitive() = default;
		virtual ~IRenderPrimitive() = default;

		void AddRef() const;
		void Release() const;

	private:
		mutable TAtomic<usize> m_ReferenceCount{1};
	};
}