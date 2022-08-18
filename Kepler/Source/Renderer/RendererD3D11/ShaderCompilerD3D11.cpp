#include "ShaderCompilerD3D11.h"
#include "Core/Core.h"
#include "Async/Async.h"
#include "Core/Filesystem/FileUtils.h"

#include "HLSLShaderD3D11.h"
#include "Renderer/RenderThread.h"

namespace Kepler
{
	TRef<TShader> THLSLShaderCompilerD3D11::CompileShader(const std::string& Path, EShaderStageFlags TypeMask)
	{
		std::string Source;
		try
		{
			Source = Await(TFileUtils::ReadTextFileAsync(Path));
		}
		catch (const TException& Exception)
		{
			KEPLER_ERROR_STOP(LogShaderCompiler, "Failed to compile shader {}.\n{}", Path, Exception.GetErrorMessage());
			return nullptr;
		}

		const auto Stages = TypeMask.Separate();
		TDynArray<std::future<TShaderModule>> ModuleFutures;
		for (const EShaderStageFlags::Type Stage : Stages)
		{
			ModuleFutures.EmplaceBack(Async(
				[PathRef = std::ref(Path), SourceRef = std::ref(Source), Stage]()
				{
					return CreateShaderModule(PathRef, Stage, SourceRef);
				})
			);
		}

		TDynArray<TShaderModule> Modules;
		Modules.Reserve(ModuleFutures.GetLength());
		for (auto& Future : ModuleFutures)
		{
			Modules.EmplaceBack(Await(Future));
		}

		return New<THLSLShaderD3D11>(Path, Modules);
	}

	TShaderModule THLSLShaderCompilerD3D11::CreateShaderModule(const std::string& SourceName, EShaderStageFlags::Type Flag, const std::string& Source)
	{
		TShaderModule OutShaderModule{};
		OutShaderModule.StageFlags = Flag;

		const std::string EntryPoint = std::invoke([Flag]
		{
			switch (Flag)
			{
			case Kepler::EShaderStageFlags::Vertex:
				return "VSMain";
			case Kepler::EShaderStageFlags::Pixel:
				return "PSMain";
			case Kepler::EShaderStageFlags::Compute:
				return "CSMain";
			default:
				break;
			}
			CHECK(false && "Unknown shader type");
		});

		OutShaderModule.ByteCode = CompileHLSLCode(SourceName, EntryPoint, Flag, Source);
		return OutShaderModule;
	}

	TRef<TDataBlob> THLSLShaderCompilerD3D11::CompileHLSLCode(const std::string& SourceName,
		const std::string& EntryPoint,
		EShaderStageFlags::Type Type,
		const std::string& Code)
	{
		KEPLER_TRACE(LogShaderCompiler, "Compiling shader '{}' as {}", SourceName, EShaderStageFlags::ToString(Type));

		std::string ShaderType;
		switch (Type)
		{
		case Kepler::EShaderStageFlags::Vertex:
			ShaderType = "vs_5_0";
			break;
		case Kepler::EShaderStageFlags::Pixel:
			ShaderType = "ps_5_0";
			break;
		case Kepler::EShaderStageFlags::Compute:
			ShaderType = "cs_5_0";
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
				CHECKMSG(false, fmt::format("Failed to compile {} shader: {}", EShaderStageFlags::ToString(Type), (const char*)(ErrorBlob->GetBufferPointer())));
			}
		}
		else
		{
			return TDataBlob::CreateGraphicsDataBlob(Blob->GetBufferPointer(), Blob->GetBufferSize());
		}
		return nullptr;
	}
}

