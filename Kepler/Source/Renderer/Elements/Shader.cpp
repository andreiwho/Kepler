#include "Shader.h"

namespace Kepler
{
	TShader::TShader(const TString& InName, const TDynArray<TShaderModule>& ShaderModules)
		:	Name(InName)
	{
		// Combine the shader stage mask
		for (const auto& Module : ShaderModules)
		{
			if (Module.ByteCode)
			{
				ShaderStageMask = ShaderStageMask | Module.StageFlags;
			}

			if (Module.StageFlags & EShaderStageFlags::Vertex)
			{
				TempVertexShaderBytecode = Module.ByteCode;
			}
		}
	}
}