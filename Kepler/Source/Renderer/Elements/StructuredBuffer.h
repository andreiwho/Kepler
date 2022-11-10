#pragma once
#include "Buffer.h"

namespace ke
{
	class IStructuredBuffer : public IBuffer
	{
	protected:
		usize m_UnitSize{};
		usize m_UnitCount{};
	};
}