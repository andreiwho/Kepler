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
		virtual RefPtr<IShader> CompileShader(const String& Path, EShaderStageFlags TypeMask) override;

	private:
		static ShaderModule CreateShaderModule(const String& SourceName, EShaderStageFlags::Type Flags, const String& Source, ID3DInclude* pInclude);
		static RefPtr<IAsyncDataBlob> CompileHLSLCode(const String& SourceName, const String& EntryPoint, EShaderStageFlags::Type Type, const String& Code, ID3DInclude* pInclude);
		static String MakeBufferSlotString(i32 index);
	};
}