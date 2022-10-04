#include "HLSLShader.h"

namespace ke
{
	THLSLShader::THLSLShader(const TString& Name, const Array<TShaderModule>& Modules)
		:	TShader(Name, Modules)
	{
	}
}