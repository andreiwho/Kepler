#pragma once
#include "Core/Core.h"

namespace physx
{
	class PxShape;
}

namespace ke
{
	class BoxShape : public Object
	{
	public:
		BoxShape(physx::PxShape* pShape);

	private:
		physx::PxShape* m_Shape;
	};
}