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
		m_UnknownIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_Unknown.png");

		// Navigation
		m_NavUpIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_NavUp.png");
		m_NavBackIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_NavBack.png");
		m_NavFwdIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_NavForward.png");
	}

	void TAssetBrowserPanel::DrawAddressBar()
	{
		if (!m_CurrentDirectory)
		{
			return;
		}

		/************************************************************************/
		/* NAV BUTTONS                                                          */
		/************************************************************************/
		// We cannot go back if current directory is root
		// UP
		DrawNavButton(m_NavUpIcon, "##navup", m_CurrentDirectory->IsRoot(), &TAssetBrowserPanel::OnTreeNavigateUp);
		ImGui::SameLine();
		DrawNavButton(m_NavBackIcon, "##navback", m_BackStack.IsEmpty(), &TAssetBrowserPanel::OnTreeNavigateBack);
		ImGui::SameLine();
		DrawNavButton(m_NavFwdIcon, "##navfwd", m_ForwardStack.IsEmpty(), &TAssetBrowserPanel::OnTreeNavigateForward);
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
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, m_ItemRounding * 3);
		if (ImGui::Selectable(visibleLabel.c_str(), m_SelectionIndexCache[itemIndex], ImGuiSelectableFlags_AllowDoubleClick, ImVec2(m_IconSize + m_IconPadding * 2, m_IconSize)))
		{
			ZeroSelectionCache();
			m_SelectionIndexCache[itemIndex] = true;

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				OnDoubleClick(itemIndex);
			}
		}
		ImGui::PopStyleVar();

		ImGui::SetCursorPos(ImVec2(cursorPos.x + m_IconPadding * 0.5f, cursorPos.y));
		ImGui::Image((ImTextureID)icon->GetNativeHandle(), ImVec2(m_IconSize, m_IconSize));


		const i32 textSize = (i32)ImGui::CalcTextSize(label.data()).x;
		i32 itemSize = m_IconSize + m_IconPadding;
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + (m_IconSize + m_IconPadding * 0.5f));
		auto offset = (itemSize - textSize) * 0.5f;
		if (offset > 0)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		}
		ImGui::TextWrapped(label.data());
		ImGui::EndGroup();
	}

	void TAssetBrowserPanel::OnDoubleClick(i32 itemIndex)
	{
		TRef<AssetTreeNode> item = m_CurrentDirectory->GetChildren()[itemIndex];
		if (item->IsDirectory())
		{
			m_BackStack.AppendBack(m_CurrentDirectory);
			m_CurrentDirectory = RefCast<AssetTreeNode_Directory>(item).Raw();
			m_ForwardStack.Clear();
		}
	}

	void TAssetBrowserPanel::OnTreeNavigateUp()
	{
		if (m_CurrentDirectory->IsRoot())
		{
			return;
		}

		AssetTreeNode_Directory* pLastDir = m_CurrentDirectory;
		m_BackStack.AppendBack(pLastDir);
		m_ForwardStack.Clear();
		m_CurrentDirectory = (AssetTreeNode_Directory*)m_CurrentDirectory->GetParent();
	}

	void TAssetBrowserPanel::OnTreeNavigateBack()
	{
		if (m_BackStack.IsEmpty())
		{
			return;
		}

		AssetTreeNode_Directory* pLastDir = m_CurrentDirectory;
		AssetTreeNode_Directory* pNextDir = m_BackStack.GetBack();
		m_CurrentDirectory = pNextDir;
		m_ForwardStack.AppendBack(pLastDir);
		m_BackStack.PopBack();
	}

	void TAssetBrowserPanel::OnTreeNavigateForward()
	{
		if (m_ForwardStack.IsEmpty())
		{
			return;
		}

		AssetTreeNode_Directory* pLastDir = m_CurrentDirectory;
		AssetTreeNode_Directory* pNextDir = m_ForwardStack.GetBack();
		m_CurrentDirectory = pNextDir;
		m_BackStack.AppendBack(pLastDir);
		m_ForwardStack.PopBack();
	}

	void TAssetBrowserPanel::DrawNavButton(TRef<TTextureSampler2D> pIcon, const char* pId, bool bDisabled, void(TAssetBrowserPanel::* pCallback)())
	{
		if (bDisabled)
		{
			ImGui::BeginDisabled(bDisabled);
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m_ItemRounding);
		if (ImGui::ImageButton(pId, (ImTextureID)pIcon->GetNativeHandle(), ImVec2(m_NavIconSize, m_NavIconSize)))
		{
			(this->*pCallback)();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if (bDisabled)
		{
			ImGui::EndDisabled();
		}
	}

	void TAssetBrowserPanel::DrawAssetTree()
	{
		if (!m_RootNode)
		{
			if (auto node = AssetManager::Get()->GetRootNode("Game://"))
			{
				m_RootNode = node.Raw();
			}

		}

		if (VALIDATED(m_RootNode))
		{
			const float desiredSize = ImGui::GetContentRegionAvail().x * 0.2f;
			const float maxSize = 200.0f;
			ImGui::BeginChild("##assettree", ImVec2(std::min(desiredSize, maxSize), 0));
			DrawAssetTreeNode(m_RootNode);
			ImGui::EndChild();
		}

	}

	void TAssetBrowserPanel::DrawAssetTreeNode(AssetTreeNode_Directory* pDirectory)
	{
		if (!pDirectory)
		{
			return;
		}

		u32 flags = m_CurrentDirectory == pDirectory ? ImGuiTreeNodeFlags_Selected : 0;
		if (pDirectory->IsRoot())
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		
		if (ImGui::TreeNodeEx(pDirectory->GetName().c_str(), flags))
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDirectory = pDirectory;
			}

			for (auto& pChild : pDirectory->GetChildren())
			{
				if (pChild && pChild->IsDirectory())
				{
					DrawAssetTreeNode(RefCast<AssetTreeNode_Directory>(pChild).Raw());
				}
			}
			ImGui::TreePop();
		}

	}

	void TAssetBrowserPanel::Draw()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(-1.0f, -1.0f));
		ImGui::Begin("Asset Browser");
		DrawAddressBar();
		DrawAssetTree();
		ImGui::SameLine();

		auto regionAvail = ImGui::GetContentRegionAvail();
		i32 widgetsPerRow = (i32)regionAvail.x / ((i32)m_IconSize + (i32)m_IconPadding);
		if (widgetsPerRow < 1)
		{
			widgetsPerRow = 1;
		}

		if (m_CurrentDirectory)
		{
			if (m_SelectionIndexCache.GetLength() != m_CurrentDirectory->GetChildren().GetLength())
			{
				m_SelectionIndexCache.Clear();
				m_SelectionIndexCache.Resize(m_CurrentDirectory->GetChildren().GetLength());
			}
			ImGui::BeginChild("##assetbrowserchild", ImVec2(0.0f, 0));
			if (ImGui::BeginTable("assetbrowser", widgetsPerRow,
				ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollY
				| ImGuiTableFlags_PadOuterX))
			{
				i32 colIndex = 0;
				i32 itemIndex = 0;
				ImGui::TableNextRow();
				for (const auto& entry : m_CurrentDirectory->GetChildren())
				{
					std::string_view view(entry->GetName());
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
					default:
					{
						DrawAsset(view, itemIndex, m_UnknownIcon);
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
				ImGui::EndChild();
			}
		}
		ImGui::End();
	}

}