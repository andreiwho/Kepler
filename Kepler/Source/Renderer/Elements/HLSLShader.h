#pragma once
#include "Core/Core.h"
#include "Shader.h"

namespace Kepler
{
	class THLSLShader : public TShader 
	{
	public:
		THLSLShader(const std::string& Name, const TDynArray<TShaderModule>& Modules);
	};
}