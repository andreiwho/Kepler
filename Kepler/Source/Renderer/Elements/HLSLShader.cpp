#include "HLSLShader.h"

namespace Kepler
{
	THLSLShader::THLSLShader(const std::string& Name, const TDynArray<TShaderModule>& Modules)
		:	TShader(Name, Modules)
	{
	}
}