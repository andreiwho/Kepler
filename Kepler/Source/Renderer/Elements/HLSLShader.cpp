#include "HLSLShader.h"

namespace ke
{
	HLSLShader::HLSLShader(const String& name, const Array<ShaderModule>& modules)
		:	IShader(name, modules)
	{
	}
}