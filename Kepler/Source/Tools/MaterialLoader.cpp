#include "MaterialLoader.h"
#include "Core/Filesystem/FileUtils.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Renderer/RenderThread.h"
#include "ImageLoader.h"
#include "Renderer/Elements/Texture.h"
#include "Renderer/LowLevelRenderer.h"
#include "Renderer/HLSLShaderCompiler.h"

#ifdef GetObject
# undef GetObject
#endif

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMaterialLoader, All);

	namespace
	{
		TString CaseInsensitive(const TString& string)
		{
			TString outString = string;
			for (auto& ch : outString)
			{
				ch = std::tolower(ch);
			}
			return outString;
		}

		//////////////////////////////////////////////////////////////////////////
		EPrimitiveTopology ParsePrimitiveTopology(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto String = Object.GetString();
			if (TString("Triangles") == String)
			{
				return EPrimitiveTopology::TriangleList;
			}
			if (TString("Lines") == String)
			{
				return EPrimitiveTopology::LineList;
			}
			if (TString("Points") == String)
			{
				return EPrimitiveTopology::PointList;
			}
			CRASH();
		}

		//////////////////////////////////////////////////////////////////////////
		EPrimitiveFillMode ParseFillMode(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto String = Object.GetString();
			if (TString("Solid") == String)
			{
				return EPrimitiveFillMode::Solid;
			}
			if (TString("Wireframe") == String)
			{
				return EPrimitiveFillMode::Wireframe;
			}
			CRASH();
		}

		EDepthComparissonMode ParseDepthMode(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto string = Object.GetString();
			if (TString("None") == string)
			{
				return EDepthComparissonMode::None;
			}
			if (TString("Less") == string)
			{
				return EDepthComparissonMode::Less;
			}
			if (TString("LEqual") == string)
			{
				return EDepthComparissonMode::LEqual;
			}
			if (TString("Equal") == string)
			{
				return EDepthComparissonMode::Equal;
			}
			if (TString("Greater") == string)
			{
				return EDepthComparissonMode::Greater;
			}
			if (TString("GEqual") == string)
			{
				return EDepthComparissonMode::GEqual;
			}
			if (TString("Always") == string)
			{
				return EDepthComparissonMode::Always;
			}
			CRASH();
		}

		//////////////////////////////////////////////////////////////////////////
		EPrimitiveCullMode ParseCullMode(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto String = Object.GetString();
			if (TString("None") == String)
			{
				return EPrimitiveCullMode::None;
			}
			if (TString("Front") == String)
			{
				return EPrimitiveCullMode::Front;
			}
			if (TString("Back") == String)
			{
				return EPrimitiveCullMode::Back;
			}
			CRASH();
		}

		//////////////////////////////////////////////////////////////////////////
		EPipelineDomain ParsePipelineDomain(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto String = Object.GetString();
			if (TString("Unlit") == String)
			{
				return EPipelineDomain::Unlit;
			}
			if (TString("Lit") == String)
			{
				return EPipelineDomain::Lit;
			}
			if (TString("Other") == String)
			{
				return EPipelineDomain::Other;
			}
			// Crash on undefined or other value
			CRASH();
		}

		//////////////////////////////////////////////////////////////////////////
		template<typename Enum>
		Enum ParseAccessFlags(const rapidjson::Value& Object)
		{
			CHECK(Object.IsString());
			const auto String = Object.GetString();
			if (TString("None") == String)
			{
				return Enum::None;
			}
			if (TString("Read") == String)
			{
				return Enum::Read;
			}
			if (TString("Write") == String)
			{
				return Enum::Write;
			}
			if (TString("ReadWrite") == String)
			{
				return Enum::Read | Enum::Write;
			}
			CRASH();
		}

		//////////////////////////////////////////////////////////////////////////
		RefPtr<IGraphicsPipeline> LoadGraphicsPipeline(const rapidjson::Value& Object, const TString& MaterialPath, bool bForce = false)
		{
			CHECK(Object.IsObject());
			CHECK(Object.HasMember("Pipeline"));

			const auto& Pipeline = Object["Pipeline"];
			CHECK(Pipeline.IsObject() && Pipeline.HasMember("Shader"));

			// Read the shader
			const auto Shader = Pipeline["Shader"].GetString();
			auto ShaderCompiler = THLSLShaderCompiler::CreateShaderCompiler();
			auto ShaderRef = ShaderCompiler->CompileShader(Shader, EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
			CHECK(ShaderRef);

			// Conigure the pipeline
			TGraphicsPipelineConfiguration PipelineConfig{};
			// Read shader domain
			if (Pipeline.HasMember("Domain"))
			{
				PipelineConfig.Domain = ParsePipelineDomain(Pipeline["Domain"]);
			}

			// Vertex Input stage
			PipelineConfig.VertexInput.VertexLayout = ShaderRef->GetReflection()->VertexLayout;
			PipelineConfig.VertexInput.Topology =
				Pipeline.HasMember("PrimitiveTopology")
				? ParsePrimitiveTopology(Pipeline["PrimitiveTopology"])
				: EPrimitiveTopology::TriangleList;
			// Rasterizer stage
			if (Pipeline.HasMember("Rasterizer"))
			{
				const auto& Rasterizer = Pipeline["Rasterizer"];
				if (Rasterizer.HasMember("bDisabled"))
				{
					const auto& Disabled = Rasterizer["bDisabled"];
					CHECK(Disabled.IsBool());
					PipelineConfig.Rasterizer.bRasterDisabled = Disabled.GetBool();
				}
				if (Rasterizer.HasMember("FillMode"))
				{
					PipelineConfig.Rasterizer.FillMode = ParseFillMode(Rasterizer["FillMode"]);
				}
				if (Rasterizer.HasMember("CullMode"))
				{
					PipelineConfig.Rasterizer.CullMode = ParseCullMode(Rasterizer["CullMode"]);
				}
				if (Rasterizer.HasMember("bEnableScissor"))
				{
					const auto& EnableScissor = Rasterizer["bEnableScissor"];
					CHECK(EnableScissor.IsBool());
					PipelineConfig.Rasterizer.bEnableScissor = EnableScissor.GetBool();
				}
			}
			// Depth stencil stage
			if (Pipeline.HasMember("DepthStencil"))
			{
				const auto& DepthStencil = Pipeline["DepthStencil"];
				if (DepthStencil.HasMember("bDepthEnable"))
				{
					PipelineConfig.DepthStencil.bDepthEnable = DepthStencil["bDepthEnable"].GetBool();
				}
				if (DepthStencil.HasMember("bStencilEnable"))
				{
					PipelineConfig.DepthStencil.bStencilEnable = DepthStencil["bStencilEnable"].GetBool();
				}
				if (DepthStencil.HasMember("DepthAccess"))
				{
					PipelineConfig.DepthStencil.DepthAccess = ParseAccessFlags<EDepthBufferAccess>(DepthStencil["DepthAccess"]);
				}
				if (DepthStencil.HasMember("StencilAccess"))
				{
					PipelineConfig.DepthStencil.StencilAccess = ParseAccessFlags<EStencilBufferAccess>(DepthStencil["StencilAccess"]);
				}
				if (DepthStencil.HasMember("DepthFunc"))
				{
					PipelineConfig.DepthStencil.DepthFunc = ParseDepthMode(DepthStencil["DepthFunc"]);
				}
			}

			if (Pipeline.HasMember("bUsePrepass"))
			{
				const auto& bUsePrepass = Pipeline["bUsePrepass"];
				CHECK(bUsePrepass.IsBool());
				PipelineConfig.bUsePrepass = bUsePrepass.GetBool();
			}

			PipelineConfig.ParamMapping = ShaderRef->GetReflection()->ParamMapping;
			return MakeRef(New<IGraphicsPipeline>(ShaderRef, PipelineConfig));
		}

		bool LoadMaterialSamplers(const rapidjson::Value& MaterialInfo, RefPtr<TMaterial> Material)
		{
			if (!MaterialInfo.IsObject() || !MaterialInfo.HasMember("Samplers"))
			{
				return false;
			}

			const auto& Samplers = MaterialInfo["Samplers"];
			auto SamplersArray = Samplers.GetObject();

			auto Mappings = Material->GetSamplers()->GetParamMappings();
			for (const auto& [Name, Sampler] : Mappings->GetSamplers())
			{
				CHECKMSG(SamplersArray.HasMember(Name.c_str()), fmt::format("Material specification file does not contain a definition for sampler '{}'. Please specify.", Name));
				const auto& Sampler = SamplersArray[Name.c_str()];

				CHECK(Sampler.IsString());
				auto LoadSampler = Sampler.GetString();
				auto LoadedSampler = TImageLoader::Get()->LoadSamplerCached(LoadSampler);
				CHECK(LoadedSampler);
				Material->WriteSampler(Name, LoadedSampler);
			}
			return true;
		}
	}

	TMaterialLoader* TMaterialLoader::Instance = nullptr;

	TMaterialLoader::TMaterialLoader()
	{
		Instance = this;
	}

	TMaterialLoader::~TMaterialLoader()
	{
	}

	void TMaterialLoader::ClearLoadedMaterialCache()
	{
	}

	RefPtr<TMaterial> TMaterialLoader::LoadMaterial(const TString& Path, bool bForce)
	{
		if (!TFileUtils::PathExists(Path))
		{
			return nullptr;
		}

		auto JsonFile = Await(TFileUtils::ReadTextFileAsync(Path));
		rapidjson::Document Document;
		Document.Parse(JsonFile.c_str());
		if (Document.HasParseError())
		{
			const auto ParseError = Document.GetParseError();
			CRASHMSG("Failed to parse json");
		}

		CHECK(Document.IsObject());
		CHECK(Document.HasMember("Material"));

		RefPtr<TMaterial> Material;
		if (!bForce)
		{
			if (TGraphicsPipelineCache::Get()->Exists(Path))
			{
				auto Pipeline = TGraphicsPipelineCache::Get()->GetPipeline(Path);
				Material = TMaterial::New(Pipeline, Path);
			}
		}
		
		if (bForce || !Material)
		{
			auto Pipeline = Await(TRenderThread::Submit([&Document, &Path, bForce] { return LoadGraphicsPipeline(Document["Material"], Path, bForce); }));
			TGraphicsPipelineCache::Get()->Add(Path, Pipeline);
			Material = TMaterial::New(Pipeline, Path);
			LoadMaterialSamplers(Document["Material"], Material);
		}

		LoadMaterialSamplers(Document["Material"], Material);

		Material->GetPipeline()->Validate();
		return Material;
	}

}