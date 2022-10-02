#include "HLSLShader.h"

namespace ke
{
	THLSLShader::THLSLShader(const TString& Name, const TDynArray<TShaderModule>& Modules)
		:	TShader(Name, Modules)
	{
	}
}