#pragma once
#include "VertexLayout.h"
#include "Core/Malloc.h"

namespace ke
{
	class TPipelineParamMapping;
	struct TShaderModuleReflection : public IntrusiveRefCounted
	{
		TVertexLayout VertexLayout{};
		RefPtr<TPipelineParamMapping> ParamMapping;
	};
}