#include "AssetBrowserPanel.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"
#include "imgui.h"
#include <string_view>
#include "Tools/ImageLoader.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogAssetBrowserPanel, All);

	TAssetBrowserPanel::TAssetBrowserPanel()
	{
		if (auto path = AssetManager::Get()->GetRootNode("Game://"))
		{
			m_CurrentDirectory = path.Raw();
		}

		// Assets
		m_FolderIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_Folder.png");
		m_FileIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_File.png");

		// Navigation
		m_NavBackIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_NavBack.png");
		m_NavFwdIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_NavForward.png");
	}

	void TAssetBrowserPanel::DrawAddressBar()
	{
		if (!m_CurrentDirectory)
		{
			return;
		}

		// We cannot go back if current directory is root
		bool bDisabled = m_CurrentDirectory->IsRoot();
		if (bDisabled)
		{
			ImGui::BeginDisabled(bDisabled);
		}

		if (ImGui::ImageButton("##navback", (ImTextureID)m_NavBackIcon->GetNativeHandle(), ImVec2(50, 50)))
		{
			if (m_CurrentDirectory->IsRoot())
			{
				return;
			}
			
			m_CurrentDirectory = (AssetTreeNode_Directory*)m_CurrentDirectory->GetParent();
		}

		if (bDisabled)
		{
			ImGui::EndDisabled();
		}

		// TODO: Add forward button

		ImGui::SameLine();

		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Text(m_CurrentDirectory->GetPath_Resolved().c_str());
	}

	void TAssetBrowserPanel::ZeroSelectionCache()
	{
		for (auto& index : m_SelectionIndexCache)
		{
			index = false;
		}
	}

	void TAssetBrowserPanel::DrawAsset(std::string_view label, i32 itemIndex, TRef<TTextureSampler2D> icon)
	{
		ImGui::BeginGroup();
		TString visibleLabel = fmt::format("##{}", label.data());
		const ImVec2 cursorPos = ImGui::GetCursorPos();
		if (ImGui::Selectable(visibleLabel.c_str(), m_SelectionIndexCache[itemIndex], ImGuiSelectableFlags_AllowDoubleClick, ImVec2(m_IconSize, m_IconSize)))
		{
			ZeroSelectionCache();
			m_SelectionIndexCache[itemIndex] = true;

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				OnDoubleClick(itemIndex);
			}
		}

		ImGui::SetCursorPos(cursorPos);
		ImGui::Image((ImTextureID)icon->GetNativeHandle(), ImVec2(m_IconSize, m_IconSize));

		const i32 textSize = (i32)ImGui::CalcTextSize(label.data()).x;
		i32 itemSize = m_IconSize;
		auto offset = ImGui::GetCursorPosX() + (itemSize - textSize) * 0.5f;
		if (offset < 0)
		{
			offset = 0;
		}
		ImGui::SetCursorPosX(offset);
		ImGui::TextWrapped(label.data());
		ImGui::EndGroup();
	}

	void TAssetBrowserPanel::OnDoubleClick(i32 itemIndex)
	{
		TRef<AssetTreeNode> item = m_CurrentDirectory->GetChildren()[itemIndex];
		if (item->IsDirectory())
		{
			m_CurrentDirectory = RefCast<AssetTreeNode_Directory>(item).Raw();
		}
	}

	void TAssetBrowserPanel::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("Asset Browser");
		DrawAddressBar();

		auto regionAvail = ImGui::GetContentRegionAvail();
		i32 widgetsPerRow = (i32)regionAvail.x / ((i32)m_IconSize + (i32)m_IconPadding);

		if (m_CurrentDirectory)
		{
			if (m_SelectionIndexCache.GetLength() != m_CurrentDirectory->GetChildren().GetLength())
			{
				m_SelectionIndexCache.Clear();
				m_SelectionIndexCache.Resize(m_CurrentDirectory->GetChildren().GetLength());
			}

			if (ImGui::BeginTable("assetbrowser", widgetsPerRow))
			{
				i32 colIndex = 0;
				i32 itemIndex = 0;
				ImGui::TableNextRow();
				for (const auto& entry : m_CurrentDirectory->GetChildren())
				{
					const auto& path = entry->GetPath_Resolved();
					usize slashIndex = path.find_last_of("/");
					std::string_view view(path.c_str() + slashIndex + 1);

					ImGui::TableSetColumnIndex(colIndex);
					switch (entry->GetNodeType())
					{
					case EAssetNodeType::Directory:
					{
						DrawAsset(view, itemIndex, m_FolderIcon);
					}
					break;
					case EAssetNodeType::PlainAsset:
					{
						DrawAsset(view, itemIndex, m_FileIcon);
					}
					break;
					}

					if (++colIndex > (widgetsPerRow - 1))
					{
						colIndex = 0;
						ImGui::TableNextRow();
					}

					itemIndex++;
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

}