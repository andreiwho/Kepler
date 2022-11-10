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
		void AllocateDataStorate(RefPtr<TShaderModuleReflection> pReflection);

		Map<String, PipelineParam> m_Params{};
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