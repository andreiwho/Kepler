#include "MaterialEditor.h"
#include <imgui.h>
#include "../Widgets/Elements.h"
#include "Renderer/Elements/Shader.h"
#include "Renderer/HLSLShaderCompiler.h"
#include "Renderer/Elements/ShaderReflection.h"
#include "Renderer/Pipelines/ParamPack.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMaterialEditor, All);

	MaterialEditor::MaterialEditor()
	{
		m_Template.OnShaderUpdated.Bind(this, &MaterialEditor::TemplateShaderUpdated);
	}


	namespace
	{
		void DrawMaterialParam(const String& name, const PipelineParam& paramInfo, void* pParamAddress)
		{
			switch (paramInfo.GetType())
			{
			case EShaderInputType::Int:
				TEditorElements::NextFieldRow(name.c_str());
				ImGui::DragInt(name.c_str(), (int*)pParamAddress);
				break;
			case EShaderInputType::Float2:
				TEditorElements::NextFieldRow(name.c_str());
				TEditorElements::DragFloat2(name.c_str(), *(float2*)pParamAddress);
				break;
			case EShaderInputType::Float3:
				TEditorElements::NextFieldRow(name.c_str());
				TEditorElements::DragFloat3(name.c_str(), *(float3*)pParamAddress);
				break;
			case EShaderInputType::Float4:
				TEditorElements::NextFieldRow(name.c_str());
				TEditorElements::DragFloat4(name.c_str(), *(float4*)pParamAddress);
				break;
			default:
				break;
			}
		}

		void DrawMaterialSampler(const String& name)
		{

		}
	}


	void MaterialEditor::Draw()
	{
		if (m_bIsOpened)
		{
			bool bBegan = ImGui::Begin("Material Editor", &m_bIsOpened);
			if (!bBegan)
			{
				ImGui::End();
				return;
			}

			if (auto pTemplateClass = GetReflectedClass<MaterialTemplate>())
			{
				TEditorElements::DrawReflectedObjectFields("Material", pTemplateClass->GetClassId(), &m_Template);
			}

			if (!m_Shader)
			{
				ImGui::End();
				return;
			}

			if (TEditorElements::Container("Params"))
			{
				if (TEditorElements::BeginFieldTable("details", 2))
				{
					if (auto pReflection = m_Shader->GetReflection())
					{
						if (auto pParams = pReflection->ParamMapping)
						{
							for (auto& [name, param] : pParams->GetParams())
							{
								DrawMaterialParam(name, param, m_Template.GetParamAddress(name));
							}
						}
					}

					TEditorElements::EndFieldTable();
				}
			}

			ImGui::End();
		}
	}

	void MaterialEditor::TemplateShaderUpdated(RefPtr<IShader> pShader)
	{
		m_Shader = pShader;
	}

	void* MaterialTemplate::GetParamAddress(const String& param)
	{
		CHECK(m_Params.Contains(param));
		auto& foundParam = m_Params[param];
		return m_DataStorage.GetData() + foundParam.GetOffset();
	}

	void MaterialTemplate::Shader_Set(AssetTreeNode* pShader)
	{
		if (!pShader || Shader == pShader)
		{
			return;
		}
		Shader = pShader;

		RefPtr<IShader> pLoadedShader = THLSLShaderCompiler::CreateShaderCompiler()->CompileShader(pShader->GetPath(), {});
		if (!pLoadedShader)
		{
			return;
		}

		if (RefPtr<TShaderModuleReflection> pReflection = pLoadedShader->GetReflection())
		{
			AllocateDataStorage(pReflection);
			m_Params = pReflection->ParamMapping->GetParams();
		}

		OnShaderUpdated.Invoke(std::move(pLoadedShader));
	}

	void MaterialTemplate::AllocateDataStorage(RefPtr<TShaderModuleReflection> pReflection)
	{
		if (auto pMapping = pReflection->ParamMapping)
		{
			struct TLocalParam
			{
				String Name;
				PipelineParam Param;
			};

			Array<TLocalParam> SortedParams;
			SortedParams.Reserve(pMapping->GetParams().GetLength());

			for (auto& [Name, Value] : pMapping->GetParams())
			{
				SortedParams.EmplaceBack(TLocalParam{ Name, Value });
			}
			SortedParams.Sort(
				[](const TLocalParam& Lhs, const TLocalParam& Rhs)
				{
					return Lhs.Param.GetOffset() < Rhs.Param.GetOffset();
				});

			// Allocate space
			const PipelineParam& LastParam = SortedParams[SortedParams.GetLength() - 1].Param;
			m_DataStorage.Resize(LastParam.GetOffset() + LastParam.GetSize());
		}
	}

}