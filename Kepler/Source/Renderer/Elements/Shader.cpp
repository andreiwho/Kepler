#include "Shader.h"

namespace ke
{
	TShaderCache* TShaderCache::Instance;

	bool TShaderCache::Exists(const String& name) const
	{
		return m_LoadedShaders.Contains(name);
	}

	void TShaderCache::Add(const String& name, RefPtr<IShader> pShader)
	{
		m_LoadedShaders.Insert(name, pShader);
	}

	RefPtr<IShader> TShaderCache::GetShader(const String& name) const
	{
		CHECK(m_LoadedShaders.Contains(name));
		return m_LoadedShaders[name];
	}

	void TShaderCache::Invalidate()
	{
		m_LoadedShaders.Clear();
	}

	IShader::IShader(const String& name, const Array<ShaderModule>& shaderModules)
		:	m_Name(name)
	{
		// Combine the shader stage mask
		for (const auto& Module : shaderModules)
		{
			if (Module.ByteCode)
			{
				m_ShaderStageMask = m_ShaderStageMask | Module.StageFlags;
			}

			if (Module.StageFlags & EShaderStageFlags::Vertex)
			{
				m_TempVertexShaderBytecode = Module.ByteCode;
			}
		}
	}
}