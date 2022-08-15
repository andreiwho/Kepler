#pragma once
#include "Renderer/ShaderCompiler.h"
#include "D3D11Common.h"

namespace Kepler
{
	class TShaderCompilerD3D11 : public TShaderCompiler
	{
	public:
		virtual TRef<TDataBlob> CompileHLSLCode(const std::string& SourceName, 
			const std::string& EntryPoint, 
			EShaderType Type,
			const std::string& Code) const override;
	};
}