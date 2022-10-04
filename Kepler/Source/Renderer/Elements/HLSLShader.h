#pragma once
#include "Core/Core.h"
#include "Shader.h"

namespace ke
{
	class THLSLShader : public TShader 
	{
	public:
		THLSLShader(const TString& Name, const Array<TShaderModule>& Modules);
	};
}