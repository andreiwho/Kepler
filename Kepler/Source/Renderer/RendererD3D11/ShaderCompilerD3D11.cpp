#include "ShaderCompilerD3D11.h"

namespace Kepler
{
	TRef<TDataBlob> TShaderCompilerD3D11::CompileHLSLCode(const std::string& SourceName, 
		const std::string& EntryPoint,
		EShaderType Type,
		const std::string& Code) const
	{
		std::string ShaderType;
		switch (Type)
		{
		case Kepler::EShaderType::Vertex:
			ShaderType = "vs_5_0";
			break;
		case Kepler::EShaderType::Pixel:
			ShaderType = "ps_5_0";
			break;
		default:
			CHECKMSG(false, "Unknown shader type");
			break;
		}

		CComPtr<ID3DBlob> ErrorBlob;
		CComPtr<ID3DBlob> Blob;
		if (FAILED(D3DCompile(
			Code.data(),
			Code.size(),
			SourceName.c_str(),
			nullptr, nullptr,
			EntryPoint.c_str(),
			ShaderType.c_str(),
			0, 0,
			&Blob,
			&ErrorBlob)))
		{
			if (ErrorBlob)
			{
				CHECKMSG(false, fmt::format("Failed to compile shader: {}", (const char*)(ErrorBlob->GetBufferPointer())));
			}
		}
		else
		{
			return TDataBlob::CreateGraphicsDataBlob(Blob->GetBufferPointer(), Blob->GetBufferSize());
		}
		return nullptr;
	}
}

