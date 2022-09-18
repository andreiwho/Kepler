#pragma once
#include "VertexLayout.h"
#include "Core/Malloc.h"

namespace Kepler
{
	class TPipelineParamMapping;
	struct TShaderModuleReflection : public TRefCounted
	{
		TVertexLayout VertexLayout{};
		TRef<TPipelineParamMapping> ParamMapping;
	};
}