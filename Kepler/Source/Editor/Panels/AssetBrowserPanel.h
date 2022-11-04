#pragma once
#include "Core/Core.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"
#include "Renderer/Elements/Texture.h"
#include "Platform/Mouse.h"
#include "Platform/Keyboard.h"

namespace ke
{
	enum class EAssetBrowserMode
	{
		Explore,
		Save,
	};

	class TAssetBrowserPanel
	{
	public:
		TAssetBrowserPanel();
		
		void Draw();

		inline bool IsHovered() const { return m_bIsHovered || m_bIsAssetTreeHovered || m_bIsAssetGridHovered; }
		void OnMouseButton(EMouseButton button);
		void OnKey(EKeyCode key);

	private:
		void OnAssetManagerRootsUpdated();

		void DrawContents();

		void DrawAddressBar();
		void ZeroSelectionCache();
		void DrawAsset(std::string_view label, i32 itemIndex, RefPtr<ITextureSampler2D> icon);
		void OnDoubleClick(i32 itemIndex);

		void OnTreeNavigateUp();
		void OnTreeNavigateBack();
		void OnTreeNavigateForward();
		void OnNavOpenSettings();
		void DrawSettingsPopup();

		void DrawNavButton(RefPtr<ITextureSampler2D> pIcon, const char* pId, bool bDisabled, void(TAssetBrowserPanel::* pCallback)());
		void DrawAssetTree();
		void DrawAssetTreeNode(const char* pCustomName, AssetTreeNode_Directory* pDirectory);
		void DrawAddressBarAddressNode(AssetTreeNode_Directory* pDirectory);
		void UpdateCurrentDirectory(AssetTreeNode_Directory* pDirectory);

	private:
		// Serialization functions
		void SerializeConfig();
		void DeserializeConfig();

	private:
		// Icons
		RefPtr<ITextureSampler2D> m_FolderIcon;
		RefPtr<ITextureSampler2D> m_FileIcon;
		RefPtr<ITextureSampler2D> m_MapIcon;
		RefPtr<ITextureSampler2D> m_MaterialIcon;
		RefPtr<ITextureSampler2D> m_StaticMeshIcon;
		RefPtr<ITextureSampler2D> m_UnknownIcon;

		// Nav
		RefPtr<ITextureSampler2D> m_NavSettingsIcon;
		RefPtr<ITextureSampler2D> m_NavUpIcon;
		RefPtr<ITextureSampler2D> m_NavBackIcon;
		RefPtr<ITextureSampler2D> m_NavFwdIcon;

		AssetTreeNode_Directory* m_CurrentDirectory{nullptr};
		AssetTreeNode_Directory* m_GameRootNode{ nullptr };
		AssetTreeNode_Directory* m_EngineRootNode{ nullptr };
		String m_LastPath{};
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
		bool m_bDrawUnknownIcons = false;
	};
}