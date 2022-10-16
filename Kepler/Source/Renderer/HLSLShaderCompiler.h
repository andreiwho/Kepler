#pragma once
#include "Renderer/RenderTypes.h"
#include "Core/Log.h"
#include "Core/Malloc.h"
#include "Elements/Shader.h"
#include "Elements/ShaderReflection.h"
#include <future>


namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogShaderCompiler, All);

	class THLSLShaderCompiler : public IntrusiveRefCounted
	{
	public:
		static RefPtr<THLSLShaderCompiler> CreateShaderCompiler();

		virtual RefPtr<TShader> CompileShader(const TString& path, EShaderStageFlags typeMask) = 0;
	};
}