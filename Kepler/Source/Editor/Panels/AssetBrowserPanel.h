#pragma once
#include "Core/Core.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "Renderer/Elements/Texture.h"

namespace ke
{
	class TAssetBrowserPanel
	{
	public:
		TAssetBrowserPanel();
		
		void Draw();

	private:
		void DrawAddressBar();
		void ZeroSelectionCache();
		void DrawAsset(std::string_view label, i32 itemIndex, TRef<TTextureSampler2D> icon);
		void OnDoubleClick(i32 itemIndex);

		void OnTreeNavigateUp();
		void OnTreeNavigateBack();
		void OnTreeNavigateForward();

		void DrawNavButton(TRef<TTextureSampler2D> pIcon, const char* pId, bool bDisabled, void(TAssetBrowserPanel::* pCallback)());

	private:
		// Icons
		TRef<TTextureSampler2D> m_FolderIcon;
		TRef<TTextureSampler2D> m_FileIcon;
		TRef<TTextureSampler2D> m_UnknownIcon;

		// Nav
		TRef<TTextureSampler2D> m_NavUpIcon;
		TRef<TTextureSampler2D> m_NavBackIcon;
		TRef<TTextureSampler2D> m_NavFwdIcon;

		AssetTreeNode_Directory* m_CurrentDirectory{nullptr};
		
		float m_IconSize = 64;
		float m_IconPadding = 16;

		float m_NavIconSize = 32.0f;
		float m_ItemRounding = 8.0f;

		Array<i8> m_SelectionIndexCache{};

		Array<AssetTreeNode_Directory*> m_BackStack;
		Array<AssetTreeNode_Directory*> m_ForwardStack;
	};
}