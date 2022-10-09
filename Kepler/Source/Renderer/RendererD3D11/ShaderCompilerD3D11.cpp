#include "ShaderCompilerD3D11.h"
#include "Core/Core.h"
#include "Async/Async.h"
#include "Core/Filesystem/FileUtils.h"

#include "HLSLShaderD3D11.h"
#include "Renderer/RenderThread.h"
#include "HLSLIncludeResolver.h"
#include "../World/WorldRenderer.h"

namespace ke
{
	TRef<TShader> THLSLShaderCompilerD3D11::CompileShader(const TString& Path, EShaderStageFlags TypeMask)
	{
		if (TShaderCache::Get()->Exists(Path))
		{
			return TShaderCache::Get()->GetShader(Path);
		}

		TString Source;
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
		Array<std::future<TShaderModule>> ModuleFutures;
		for (const EShaderStageFlags::Type Stage : Stages)
		{
			ModuleFutures.EmplaceBack(Async(
				[PathRef = VFSResolvePath(Path), SourceRef = std::ref(Source), Stage]()
				{
					return CreateShaderModule(PathRef, Stage, SourceRef);
				})
			);
		}

		Array<TShaderModule> Modules;
		Modules.Reserve(ModuleFutures.GetLength());
		for (auto& Future : ModuleFutures)
		{
			Modules.EmplaceBack(Await(Future));
		}

		auto OutShader = New<THLSLShaderD3D11>(Path, Modules);
		TShaderCache::Get()->Add(Path, OutShader);
		return OutShader;
	}

	TShaderModule THLSLShaderCompilerD3D11::CreateShaderModule(const TString& SourceName, EShaderStageFlags::Type Flag, const TString& Source)
	{
		TShaderModule OutShaderModule{};
		OutShaderModule.StageFlags = Flag;

		const TString EntryPoint = std::invoke([Flag]
		{
			switch (Flag)
			{
			case ke::EShaderStageFlags::Vertex:
				return "VSMain";
			case ke::EShaderStageFlags::Pixel:
				return "PSMain";
			case ke::EShaderStageFlags::Compute:
				return "CSMain";
			default:
				break;
			}
			CRASHMSG("Unknown shader type");
		});

		OutShaderModule.ByteCode = CompileHLSLCode(SourceName, EntryPoint, Flag, Source);
		return OutShaderModule;
	}

	TRef<AsyncDataBlob> THLSLShaderCompilerD3D11::CompileHLSLCode(const TString& SourceName,
		const TString& EntryPoint,
		EShaderStageFlags::Type Type,
		const TString& Code)
	{
		KEPLER_TRACE(LogShaderCompiler, "Compiling shader '{}' as {}", SourceName, EShaderStageFlags::ToString(Type));

		TString ShaderType;
		switch (Type)
		{
		case ke::EShaderStageFlags::Vertex:
			ShaderType = "vs_5_0";
			break;
		case ke::EShaderStageFlags::Pixel:
			ShaderType = "ps_5_0";
			break;
		case ke::EShaderStageFlags::Compute:
			ShaderType = "cs_5_0";
			break;
		default:
			CRASHMSG("Unknown shader type");
			break;
		}

		CComPtr<ID3DBlob> ErrorBlob{};
		CComPtr<ID3DBlob> Blob;

		Array<D3D_SHADER_MACRO> shaderMacros;
		TString CameraSlot = MakeBufferSlotString(TWorldRenderer::RS_Camera);
		TString LightSlot = MakeBufferSlotString(TWorldRenderer::RS_Light);
		TString UserSlot = MakeBufferSlotString(TWorldRenderer::RS_User);

		shaderMacros.EmplaceBack(D3D_SHADER_MACRO{"RS_Camera", CameraSlot.c_str()});
		shaderMacros.EmplaceBack(D3D_SHADER_MACRO{"RS_Light", LightSlot.c_str()});
		shaderMacros.EmplaceBack(D3D_SHADER_MACRO{"RS_User", UserSlot.c_str()});
		shaderMacros.EmplaceBack(D3D_SHADER_MACRO{nullptr, nullptr});

		if (FAILED(D3DCompile(
			Code.data(),
			Code.size(),
			SourceName.c_str(),
			shaderMacros.GetData(), 
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			EntryPoint.c_str(),
			ShaderType.c_str(),
			0, 0,
			&Blob,
			&ErrorBlob)))
		{
			if (ErrorBlob)
			{
				TString Message = fmt::format("Failed to compile {} shader: {}", EShaderStageFlags::ToString(Type), (const char*)(ErrorBlob->GetBufferPointer()));
				KEPLER_ERROR_STOP(LogShaderCompiler, "{}", Message);
				CRASHMSG(fmt::format("{}", Message));
			}
		}
		else
		{
			if (ErrorBlob)
			{
				TString Message = fmt::format("Failed to compile {} shader: {}", EShaderStageFlags::ToString(Type), (const char*)(ErrorBlob->GetBufferPointer()));
				KEPLER_WARNING(LogShaderCompiler, "While compiling {} shader: {}", EShaderStageFlags::ToString(Type), Message);
			}
			return AsyncDataBlob::CreateGraphicsDataBlob(Blob->GetBufferPointer(), Blob->GetBufferSize());
		}
		return nullptr;
	}

	TString THLSLShaderCompilerD3D11::MakeBufferSlotString(i32 index)
	{
		return fmt::format("b{}", index);
	}

}

