#pragma once
#include "Core/Core.h"
#include "Shader.h"

namespace Kepler
{
	class THLSLShader : public TShader 
	{
	public:
		THLSLShader(const TString& Name, const TDynArray<TShaderModule>& Modules);
	};
}