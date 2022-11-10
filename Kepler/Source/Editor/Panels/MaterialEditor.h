#pragma once
#include "Core/Core.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "Renderer/Elements/ShaderReflection.h"
#include "Renderer/Elements/Shader.h"
#include "Renderer/Pipelines/ParamPack.h"
#include "MaterialEditor.gen.h"

namespace ke
{
	DEFINE_MULTICAST_DELEGATE_WITH_PARAMS(OnShaderUpdatedDelegate, RefPtr<IShader>);

	reflected struct SamplerReference
	{
		reflected_body();
	private:
		reflected kmeta(AssetType=Texture)
		AssetTreeNode* SamplerAsset = nullptr;
	};

	reflected struct MaterialTemplate
	{
		reflected_body();
	public:
		OnShaderUpdatedDelegate OnShaderUpdated;

		const Map<String, PipelineParam>& GetParams() const { return m_Params; }
		void* GetParamAddress(const String& param);

	private:
		reflected kmeta(assettype=Shader, set=Shader_Set)
		AssetTreeNode* Shader = nullptr;
		void Shader_Set(AssetTreeNode* pShader);

		Array<ubyte> m_DataStorage{};
		void AllocateDataStorage(RefPtr<TShaderModuleReflection> pReflection);

		Map<String, PipelineParam> m_Params{};
		Map<String, SamplerReference> m_Samplers{};
	};

	class MaterialEditor
	{
	public:
		MaterialEditor();

		inline void Open() { m_bIsOpened = true; }
		void Draw();

	private:
		void TemplateShaderUpdated(RefPtr<IShader> pShader);

	private:
		bool m_bIsOpened = false;
		MaterialTemplate m_Template{};

		RefPtr<IShader> m_Shader;
	};
}