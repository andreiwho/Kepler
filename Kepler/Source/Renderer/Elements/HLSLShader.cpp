#include "HLSLShader.h"

namespace Kepler
{
	THLSLShader::THLSLShader(const TString& Name, const TDynArray<TShaderModule>& Modules)
		:	TShader(Name, Modules)
	{
	}
}