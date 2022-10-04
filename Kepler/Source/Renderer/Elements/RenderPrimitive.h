#pragma once
#include "Core/Types.h"

#include <atomic>

namespace ke
{
	class TRenderPrimitive
	{
	public:
		TRenderPrimitive() = default;
		virtual ~TRenderPrimitive() = default;

		void AddRef() const;
		void Release() const;

	private:
		mutable TAtomic<usize> ReferenceCount{1};
	};
}