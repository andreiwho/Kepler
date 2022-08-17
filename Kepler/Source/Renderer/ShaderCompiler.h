#pragma once
#include "Renderer/RenderTypes.h"
#include "Core/Log.h"
#include "Core/Malloc.h"
#include <future>

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogShaderCompiler);

	class TShaderCompiler : public TRefCounted
	{
	public:
		static TRef<TShaderCompiler> CreateShaderCompiler();

		virtual TRef<TDataBlob> CompileHLSLCode(const std::string& SourceName,
			const std::string& EntryPoint,
			EShaderType Type,
			const std::string& Code) const = 0;

		virtual TRef<TDataBlob> CompileShaderCodeFromFile(const std::string& FilePath, const std::string& EntryPoint, EShaderType Type) const = 0;
		virtual std::future<TRef<TDataBlob>> CompileShaderCodeFromFileAsync(std::string FilePath, std::string EntryPoint, EShaderType Type) const = 0;
	};
}