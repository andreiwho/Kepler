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
	//////////////////////////////////////////////////////////////////////////
	namespace fs = std::filesystem;
	class IncludeInterface : public ID3DInclude
	{
	public:
		IncludeInterface(const String& shaderPath)
			: m_ShaderPath(shaderPath)
		{}

		STDOVERRIDEMETHODIMP Open(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
		{
			using namespace std::string_literals;

			String shaderPath = VFSResolvePath(m_ShaderPath);

			switch (IncludeType)
			{
			case D3D_INCLUDE_LOCAL:
			{
				fs::path parentPath = fs::path(shaderPath).parent_path();
				fs::path incPath = parentPath / pFileName;
				if (fs::exists(incPath))
				{
					std::scoped_lock lck{ m_Mutex };
					String includedPath = incPath.string();
					if (m_LoadedShaders.Contains(includedPath))
					{
						GetBufferPointer(includedPath, ppData, pBytes);
						return S_OK;
					}

					m_LoadedShaders[includedPath] = Await(TFileUtils::ReadTextFileAsync(includedPath, true));
					GetBufferPointer(includedPath, ppData, pBytes);
					return S_OK;
				}
			}
			break;
			case D3D_INCLUDE_SYSTEM:
			{
				String includedPath = VFSResolvePath("EngineShaders://"s + pFileName);
				if (fs::exists(includedPath))
				{
					std::scoped_lock lck{ m_Mutex };
					if (m_LoadedShaders.Contains(includedPath))
					{
						GetBufferPointer(includedPath, ppData, pBytes);
						return S_OK;
					}

					m_LoadedShaders[includedPath] = Await(TFileUtils::ReadTextFileAsync(includedPath, true));
					GetBufferPointer(includedPath, ppData, pBytes);
					return S_OK;
				}
			}
			break;
			};
			return E_INVALIDARG;
		}

		void GetBufferPointer(const String& key, LPCVOID* ppData, UINT* pBytes)
		{
			*ppData = m_LoadedShaders[key].c_str();
			*pBytes = (UINT)m_LoadedShaders[key].length() - 1;
		}

		STDOVERRIDEMETHODIMP Close(THIS_ LPCVOID pData)
		{
			return S_OK;
		}

	private:
		Map<String, String> m_LoadedShaders;
		String m_ShaderPath{};
		std::mutex m_Mutex{};
	};

	//////////////////////////////////////////////////////////////////////////
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

		EShaderStageFlags StageMask = ParseShaderStageFlags(Source);
		const auto Stages = StageMask.Separate();
		Array<std::future<ShaderModule>> ModuleFutures;
		IncludeInterface* pInclude = New<IncludeInterface>(Path);
		for (const EShaderStageFlags::Type Stage : Stages)
		{
			ModuleFutures.EmplaceBack(Async(
				[PathRef = VFSResolvePath(Path), SourceRef = std::ref(Source), Stage, pInclude]()
				{
					return CreateShaderModule(PathRef, Stage, SourceRef, pInclude);
				})
			);
		}

		Array<ShaderModule> Modules;
		Modules.Reserve(ModuleFutures.GetLength());
		for (auto& Future : ModuleFutures)
		{
			Modules.EmplaceBack(Await(Future));
		}
		Delete(pInclude);

		auto OutShader = New<THLSLShaderD3D11>(Path, Modules);
		TShaderCache::Get()->Add(Path, OutShader);
		return OutShader;
	}

	ShaderModule THLSLShaderCompilerD3D11::CreateShaderModule(const String& SourceName, EShaderStageFlags::Type Flag, const String& Source, ID3DInclude* pInclude)
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

		OutShaderModule.ByteCode = CompileHLSLCode(SourceName, EntryPoint, Flag, Source, pInclude);
		return OutShaderModule;
	}

	RefPtr<IAsyncDataBlob> THLSLShaderCompilerD3D11::CompileHLSLCode(const String& SourceName,
		const String& EntryPoint,
		EShaderStageFlags::Type Type,
		const String& Code,
		ID3DInclude* pInclude)
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
		auto& slotValues = GetReflectedEnum<EReservedSlots>()->GetEnumValues();
		Array<String> paramValues;
		paramValues.Reserve(slotValues.GetLength());
		u32 index = 0;
		for (auto& [value, name] : slotValues)
		{
			paramValues.AppendBack(fmt::format("b{}", value));
			shaderMacros.AppendBack(D3D_SHADER_MACRO{ name.c_str(), paramValues[index].c_str()});
			index++;
		}
		shaderMacros.EmplaceBack(D3D_SHADER_MACRO{ nullptr, nullptr });

		if (FAILED(D3DCompile(
			Code.data(),
			Code.size(),
			SourceName.c_str(),
			shaderMacros.GetData(),
			pInclude,
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

	EShaderStageFlags THLSLShaderCompilerD3D11::ParseShaderStageFlags(const String& shaderSource)
	{
		EShaderStageFlags outFlags{};
		if (shaderSource.find("VSMain") != String::npos)
		{
			outFlags.Mask |= EShaderStageFlags::Vertex;
		}
		if (shaderSource.find("PSMain") != String::npos)
		{
			outFlags.Mask |= EShaderStageFlags::Pixel;
		}
		if (shaderSource.find("CSMain") != String::npos)
		{
			outFlags.Mask |= EShaderStageFlags::Compute;
		}
		return outFlags;
	}

}

