#pragma once
#include "Renderer/RenderTypes.h"
#include "Core/Malloc.h"

namespace Kepler
{
	class TShaderCompiler : public TRefCounted
	{
	public:
		static TRef<TShaderCompiler> CreateShaderCompiler();

		virtual TRef<TDataBlob> CompileHLSLCode(const std::string& SourceName,
			const std::string& EntryPoint,
			EShaderType Type, 
			const std::string& Code) const = 0;
	};
}