#pragma once
#include "Core/Core.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "Renderer/Elements/Texture.h"
#include "Platform/Mouse.h"

namespace ke
{
	class TAssetBrowserPanel
	{
	public:
		TAssetBrowserPanel();
		
		void Draw();

		inline bool IsHovered() const { return m_bIsHovered || m_bIsAssetTreeHovered || m_bIsAssetGridHovered; }
		void OnMouseButton(EMouseButton button);

	private:
		void DrawAddressBar();
		void ZeroSelectionCache();
		void DrawAsset(std::string_view label, i32 itemIndex, RefPtr<TTextureSampler2D> icon);
		void OnDoubleClick(i32 itemIndex);

		void OnTreeNavigateUp();
		void OnTreeNavigateBack();
		void OnTreeNavigateForward();
		void OnNavOpenSettings();
		void DrawSettingsPopup();

		void DrawNavButton(RefPtr<TTextureSampler2D> pIcon, const char* pId, bool bDisabled, void(TAssetBrowserPanel::* pCallback)());
		void DrawAssetTree();
		void DrawAssetTreeNode(const char* pCustomName, AssetTreeNode_Directory* pDirectory);
		void DrawAddressBarAddressNode(AssetTreeNode_Directory* pDirectory);

	private:
		// Serialization functions
		void SerializeConfig();
		void DeserializeConfig();

	private:
		// Icons
		RefPtr<TTextureSampler2D> m_FolderIcon;
		RefPtr<TTextureSampler2D> m_FileIcon;
		RefPtr<TTextureSampler2D> m_UnknownIcon;

		// Nav
		RefPtr<TTextureSampler2D> m_NavSettingsIcon;
		RefPtr<TTextureSampler2D> m_NavUpIcon;
		RefPtr<TTextureSampler2D> m_NavBackIcon;
		RefPtr<TTextureSampler2D> m_NavFwdIcon;

		AssetTreeNode_Directory* m_CurrentDirectory{nullptr};
		AssetTreeNode_Directory* m_GameRootNode{ nullptr };
		AssetTreeNode_Directory* m_EngineRootNode{ nullptr };
		bool m_bShowEngineContent = true;

		float m_IconSize = 80;
		float m_IconPadding = 16;

		float m_NavIconSize = 32.0f;
		float m_ItemRounding = 8.0f;

		Array<i8> m_SelectionIndexCache{};

		Array<AssetTreeNode_Directory*> m_BackStack;
		Array<AssetTreeNode_Directory*> m_ForwardStack;

		bool m_bIsHovered = false;
		bool m_bIsAssetTreeHovered = false;
		bool m_bIsAssetGridHovered = false;
	};
}