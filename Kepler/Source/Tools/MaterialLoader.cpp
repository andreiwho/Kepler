#include "MaterialLoader.h"
#include "Core/Filesystem/FileUtils.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Renderer/Pipelines/Default/DefaultUnlitPipeline.h"
#include "Renderer/Pipelines/Default/ScreenQuadPipeline.h"
#include "Renderer/RenderThread.h"
#include "ImageLoader.h"
#include "Renderer/Elements/Texture.h"
#include "Renderer/LowLevelRenderer.h"

#ifdef GetObject
# undef GetObject
#endif

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMaterialLoader);

	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		EPipelineCategory ParsePipelineCategory(const rapidjson::Value& Object)
		{
			CHECK(Object.IsObject());
			CHECK(Object.HasMember("Pipeline"));

			const auto& PipelineName = Object["Pipeline"];
			if (PipelineName.IsString())
			{
				auto PipelineString = PipelineName.GetString();
				if (TString("DefaultUnlit") == PipelineString)
				{
					return EPipelineCategory::DefaultUnlit;
				}
				if (TString("PostProcess") == PipelineString)
				{
					return EPipelineCategory::PostProcess;
				}
			}
			KEPLER_WARNING(LogMaterialLoader, "Unknown material pipeline.");
			return EPipelineCategory::Unknown;
		}

		//////////////////////////////////////////////////////////////////////////
		TRef<TGraphicsPipeline> LoadGraphicsPipeline(const rapidjson::Value& Object)
		{
			CHECK(Object.IsObject());
			CHECK(Object.HasMember("Pipeline"));

			TString CustomShader{};
			if (Object.HasMember("CustomShader"))
			{
				CustomShader = Object["CustomShader"].GetString();
			}

			EPipelineCategory Category = ParsePipelineCategory(Object);
			CHECK(Category != EPipelineCategory::Unknown);

			switch (Category)
			{
			case EPipelineCategory::Unknown:
				CRASH();
				break;
			case EPipelineCategory::DefaultUnlit:
				return MakeRef(New<TDefaultUnlitPipeline>(CustomShader));
				break;
			case EPipelineCategory::PostProcess:
				return MakeRef(New<TScreenQuadPipeline>(CustomShader));
				break;
			default:
				break;
			}
			CRASH();
		}

		bool LoadMaterialSamplers(const rapidjson::Value& MaterialInfo, TRef<TMaterial> Material)
		{
			if (!MaterialInfo.IsObject() && !MaterialInfo.HasMember("Samplers"))
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

	TRef<TMaterial> TMaterialLoader::LoadMaterial(const TString& Path)
	{
		if (!TFileUtils::PathExists(Path))
		{
			return nullptr;
		}

		auto JsonFile = Await(TFileUtils::ReadTextFileAsync(Path));
		rapidjson::Document Document;
		Document.Parse(JsonFile.c_str());

		CHECK(Document.IsObject());
		CHECK(Document.HasMember("Material"));

		auto Pipeline = TRenderThread::Submit([&Document] { return LoadGraphicsPipeline(Document["Material"]); });
		auto Material = TMaterial::New(Await(Pipeline), Path);

		// Load material samplers
		if (LoadMaterialSamplers(Document["Material"], Material))
		{
			// Material has all samplers specified
		}
		return Material;
	}

}