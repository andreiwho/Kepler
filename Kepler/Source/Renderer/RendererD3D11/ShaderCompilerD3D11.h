#pragma once
#include "Renderer/ShaderCompiler.h"
#include "D3D11Common.h"
#include "Async/Async.h"

namespace Kepler
{
	class TShaderCompilerD3D11 : public TShaderCompiler
	{
	public:
		virtual TRef<TDataBlob> CompileHLSLCode(const std::string& SourceName,
			const std::string& EntryPoint,
			EShaderType Type,
			const std::string& Code) const override;

		virtual TRef<TDataBlob> CompileShaderCodeFromFile(const std::string& FilePath, const std::string& EntryPoint, EShaderType Type) const override;
		virtual std::future<TRef<TDataBlob>> CompileShaderCodeFromFileAsync(std::string FilePath, std::string EntryPoint, EShaderType Type) const override;

	};
}