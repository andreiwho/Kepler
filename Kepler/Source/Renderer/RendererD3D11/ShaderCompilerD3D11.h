#pragma once
#include "Renderer/HLSLShaderCompiler.h"
#include "D3D11Common.h"
#include "Async/Async.h"
#include "../Elements/VertexLayout.h"

namespace Kepler
{
	class THLSLShaderCompilerD3D11 : public THLSLShaderCompiler
	{
	public:
		virtual TRef<TShader> CompileShader(const TString& Path, 
			EShaderStageFlags TypeMask) override;

	private:
		static TShaderModule CreateShaderModule(const TString& SourceName, EShaderStageFlags::Type Flags, const TString& Source);
		static TRef<TDataBlob> CompileHLSLCode(const TString& SourceName, const TString& EntryPoint, EShaderStageFlags::Type Type, const TString& Code);
	};
}