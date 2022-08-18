#pragma once
#include "Renderer/HLSLShaderCompiler.h"
#include "D3D11Common.h"
#include "Async/Async.h"

namespace Kepler
{
	class THLSLShaderCompilerD3D11 : public THLSLShaderCompiler
	{
	public:
		virtual TRef<TShader> CompileShader(const std::string& Path, 
			EShaderStageFlags TypeMask) override;

	private:
		static TShaderModule CreateShaderModule(const std::string& SourceName, EShaderStageFlags::Type Flags, const std::string& Source);
		static TRef<TDataBlob> CompileHLSLCode(const std::string& SourceName, const std::string& EntryPoint, EShaderStageFlags::Type Type, const std::string& Code);
	};
}