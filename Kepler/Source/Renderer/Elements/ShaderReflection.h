#pragma once
#include "VertexLayout.h"
#include "Core/Malloc.h"

namespace ke
{
	class PipelineParamMapping;
	struct TShaderModuleReflection : public IntrusiveRefCounted
	{
		VertexLayout VertexLayout{};
		RefPtr<PipelineParamMapping> ParamMapping;
	};
}