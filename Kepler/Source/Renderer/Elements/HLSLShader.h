#pragma once
#include "Core/Core.h"
#include "Shader.h"

namespace ke
{
	class HLSLShader : public IShader 
	{
	public:
		HLSLShader(const String& name, const Array<ShaderModule>& modules);
	};
}