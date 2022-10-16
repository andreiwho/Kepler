#pragma once
#include "Renderer/HLSLShaderCompiler.h"
#include "D3D11Common.h"
#include "Async/Async.h"
#include "../Elements/VertexLayout.h"

namespace ke
{
	class THLSLShaderCompilerD3D11 : public THLSLShaderCompiler
	{
	public:
		virtual RefPtr<IShader> CompileShader(const TString& Path, EShaderStageFlags TypeMask) override;

	private:
		static ShaderModule CreateShaderModule(const TString& SourceName, EShaderStageFlags::Type Flags, const TString& Source);
		static RefPtr<IAsyncDataBlob> CompileHLSLCode(const TString& SourceName, const TString& EntryPoint, EShaderStageFlags::Type Type, const TString& Code);
		static TString MakeBufferSlotString(i32 index);
	};
}