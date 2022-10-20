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
	RefPtr<IShader> THLSLShaderCompilerD3D11::CompileShader(const String& Path, EShaderStageFlags TypeMask)
	{
		if (TShaderCache::Get()->Exists(Path))
		{
			return TShaderCache::Get()->GetShader(Path);
		}

		String Source;
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
		Array<std::future<ShaderModule>> ModuleFutures;
		for (const EShaderStageFlags::Type Stage : Stages)
		{
			ModuleFutures.EmplaceBack(Async(
				[PathRef = VFSResolvePath(Path), SourceRef = std::ref(Source), Stage]()
				{
					return CreateShaderModule(PathRef, Stage, SourceRef);
				})
			);
		}

		Array<ShaderModule> Modules;
		Modules.Reserve(ModuleFutures.GetLength());
		for (auto& Future : ModuleFutures)
		{
			Modules.EmplaceBack(Await(Future));
		}

		auto OutShader = New<THLSLShaderD3D11>(Path, Modules);
		TShaderCache::Get()->Add(Path, OutShader);
		return OutShader;
	}

	ShaderModule THLSLShaderCompilerD3D11::CreateShaderModule(const String& SourceName, EShaderStageFlags::Type Flag, const String& Source)
	{
		ShaderModule OutShaderModule{};
		OutShaderModule.StageFlags = Flag;

		const String EntryPoint = std::invoke([Flag]
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

	RefPtr<IAsyncDataBlob> THLSLShaderCompilerD3D11::CompileHLSLCode(const String& SourceName,
		const String& EntryPoint,
		EShaderStageFlags::Type Type,
		const String& Code)
	{
		KEPLER_TRACE(LogShaderCompiler, "Compiling shader '{}' as {}", SourceName, EShaderStageFlags::ToString(Type));

		String ShaderType;
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

		ID3DBlob* ErrorBlob{};
		ID3DBlob* Blob;

		Array<D3D_SHADER_MACRO> shaderMacros;
		String CameraSlot = MakeBufferSlotString(WorldRenderer::RS_Camera);
		String LightSlot = MakeBufferSlotString(WorldRenderer::RS_Light);
		String UserSlot = MakeBufferSlotString(WorldRenderer::RS_User);

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
				String Message = fmt::format("Failed to compile {} shader: {}", EShaderStageFlags::ToString(Type), (const char*)(ErrorBlob->GetBufferPointer()));
				KEPLER_ERROR_STOP(LogShaderCompiler, "{}", Message);
				SAFE_RELEASE(Blob);
				SAFE_RELEASE(ErrorBlob);
				CRASHMSG(fmt::format("{}", Message));
			}
		}
		else
		{
			if (ErrorBlob)
			{
				KEPLER_WARNING(LogShaderCompiler, "While compiling {} shader: {}", EShaderStageFlags::ToString(Type), (const char*)(ErrorBlob->GetBufferPointer()));
				SAFE_RELEASE(ErrorBlob);
			}
			auto outBlob = IAsyncDataBlob::CreateGraphicsDataBlob(Blob->GetBufferPointer(), Blob->GetBufferSize());
			SAFE_RELEASE(Blob);
			return outBlob;
		}
		return nullptr;
	}

	String THLSLShaderCompilerD3D11::MakeBufferSlotString(i32 index)
	{
		return fmt::format("b{}", index);
	}

}

