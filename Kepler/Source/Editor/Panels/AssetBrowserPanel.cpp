#include "AssetBrowserPanel.h"
#include "Core/Filesystem/AssetSystem/AssetManager.h"
#include "imgui.h"
#include <string_view>

namespace ke
{
	TAssetBrowserPanel::TAssetBrowserPanel()
	{
		if (auto path = AssetManager::Get()->GetRootNode("Game://"))
		{
			m_CurrentDirectory = path.Raw();
		}
	}

	void TAssetBrowserPanel::Draw()
	{
		ImGui::Begin("Asset Browser");
		if (m_CurrentDirectory)
		{
			for (const auto& entry : m_CurrentDirectory->GetChildren())
			{
				const auto& path = entry->GetPath_Resolved();
				usize slashIndex = path.find_last_of("/");
				std::string_view view(path.c_str() + slashIndex + 1);
				ImGui::Text(view.data());
			}
		}
		ImGui::End();
	}

}