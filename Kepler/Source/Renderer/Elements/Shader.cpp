#include "Shader.h"

namespace ke
{
	TShaderCache* TShaderCache::Instance;

	bool TShaderCache::Exists(const TString& Name) const
	{
		return LoadedShaders.Contains(Name);
	}

	void TShaderCache::Add(const TString& Name, RefPtr<TShader> Shader)
	{
		LoadedShaders.Insert(Name, Shader);
	}

	RefPtr<TShader> TShaderCache::GetShader(const TString& Name) const
	{
		CHECK(LoadedShaders.Contains(Name));
		return LoadedShaders[Name];
	}

	void TShaderCache::Invalidate()
	{
		LoadedShaders.Clear();
	}

	TShader::TShader(const TString& InName, const Array<TShaderModule>& ShaderModules)
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