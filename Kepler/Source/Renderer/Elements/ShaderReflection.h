#pragma once
#include "VertexLayout.h"
#include "Core/Malloc.h"

namespace Kepler
{
	struct TShaderModuleReflection : public TRefCounted
	{
		TVertexLayout VertexLayout{};
	};
}