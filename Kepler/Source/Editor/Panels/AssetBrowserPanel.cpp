#include "AssetBrowserPanel.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"
#include "imgui.h"
#include <string_view>
#include "Tools/ImageLoader.h"
#include "Core/Filesystem/FileUtils.h"

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
		m_NavSettingsIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_Settings.png");
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
		DrawNavButton(m_NavSettingsIcon, "##navsettings", false, &TAssetBrowserPanel::OnNavOpenSettings);
		ImGui::SameLine();
		DrawNavButton(m_NavUpIcon, "##navup", m_CurrentDirectory->IsRoot(), &TAssetBrowserPanel::OnTreeNavigateUp);
		ImGui::SameLine();
		DrawNavButton(m_NavBackIcon, "##navback", m_BackStack.IsEmpty(), &TAssetBrowserPanel::OnTreeNavigateBack);
		ImGui::SameLine();
		DrawNavButton(m_NavFwdIcon, "##navfwd", m_ForwardStack.IsEmpty(), &TAssetBrowserPanel::OnTreeNavigateForward);
		ImGui::SameLine();

		DrawAddressBarAddressNode(m_CurrentDirectory);
		DrawSettingsPopup();
	}

	void TAssetBrowserPanel::DrawAddressBarAddressNode(AssetTreeNode_Directory* pDirectory)
	{
		if (!pDirectory->IsRoot())
		{
			if (auto pParent = pDirectory->GetParent())
			{
				DrawAddressBarAddressNode((AssetTreeNode_Directory*)pParent);
			}
		}

		bool bDisabled = pDirectory == m_CurrentDirectory;
		if (bDisabled)
		{
			ImGui::BeginDisabled(bDisabled);
		}

		if (ImGui::Button(pDirectory->GetName().c_str(), ImVec2(0.0f, m_NavIconSize)))
		{
			m_BackStack.AppendBack(m_CurrentDirectory);
			m_ForwardStack.Clear();
			m_CurrentDirectory = pDirectory;

		}

		if (bDisabled)
		{
			ImGui::EndDisabled();
		}

		ImGui::SameLine();
		ImGui::Text("/");

		if (pDirectory != m_CurrentDirectory)
		{
			ImGui::SameLine();
		}
	}


	void TAssetBrowserPanel::SerializeConfig()
	{
		// TODO
	}

	void TAssetBrowserPanel::DeserializeConfig()
	{
		// TODO
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
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m_ItemRounding * 3);
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

	void TAssetBrowserPanel::OnNavOpenSettings()
	{
		ImGui::OpenPopup("assetbrowsersettings");
	}

	void TAssetBrowserPanel::DrawSettingsPopup()
	{
		if (ImGui::BeginPopup("assetbrowsersettings"))
		{
			ImGui::SliderFloat("Icon Size", &m_IconSize, 32.0f, 256.0f);
			ImGui::EndPopup();
		}
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
		// TODO: Checkout if we should setup additional directories
		if (!m_GameRootNode)
		{
			if (auto node = AssetManager::Get()->GetRootNode("Game://"))
			{
				m_GameRootNode = node.Raw();
			}
		}

		if (m_bShowEngineContent)
		{
			if (!m_EngineRootNode)
			{
				if (auto node = AssetManager::Get()->GetRootNode("Engine://"))
				{
					m_EngineRootNode = node.Raw();
				}
			}
		}

		const float desiredSize = ImGui::GetContentRegionAvail().x * 0.2f;
		const float maxSize = 200.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
		ImGui::BeginChild("##assettree", ImVec2(std::min(desiredSize, maxSize), 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		m_bIsAssetTreeHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		if (VALIDATED(m_GameRootNode))
		{
			DrawAssetTreeNode("Game", m_GameRootNode);
		}

		if (m_bShowEngineContent)
		{
			if (VALIDATED(m_EngineRootNode))
			{
				DrawAssetTreeNode("Engine", m_EngineRootNode);
			}
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::EndChild();
	}

	void TAssetBrowserPanel::DrawAssetTreeNode(const char* pCustomName, AssetTreeNode_Directory* pDirectory)
	{
		if (!pDirectory)
		{
			return;
		}

		u32 flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_CurrentDirectory == pDirectory)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (pDirectory->IsRoot())
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (!pDirectory->HasDirectories())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		}

		const TString name = pCustomName ? pCustomName : pDirectory->GetName();
		bool bOpened = ImGui::TreeNodeEx((name + "##" + pDirectory->GetPath_Resolved()).c_str(), flags);
		bool bClicked = ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen();

		if (bClicked)
		{
			m_BackStack.AppendBack(m_CurrentDirectory);
			m_ForwardStack.Clear();
			m_CurrentDirectory = pDirectory;
		}

		if (bOpened)
		{
			for (auto& pChild : pDirectory->GetChildren())
			{
				if (pChild && pChild->IsDirectory())
				{
					DrawAssetTreeNode(nullptr, RefCast<AssetTreeNode_Directory>(pChild).Raw());
				}
			}
		}

		if (bOpened)
		{
			ImGui::TreePop();
		}
	}

	void TAssetBrowserPanel::Draw()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(-1.0f, -1.0f));
		ImGui::Begin("Asset Browser");

		m_bIsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

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
			m_bIsAssetGridHovered = ImGui::IsWindowHovered();
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

	void TAssetBrowserPanel::OnMouseButton(EMouseButton button)
	{
		switch (button)
		{
		case EMouseButton::Back:
			OnTreeNavigateBack();
			return;
		case EMouseButton::Forward:
			OnTreeNavigateForward();
			return;
		default:
			break;
		}
	}
}