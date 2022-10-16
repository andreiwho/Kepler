#include "HLSLShader.h"

namespace ke
{
	HLSLShader::HLSLShader(const TString& name, const Array<ShaderModule>& modules)
		:	IShader(name, modules)
	{
	}
}