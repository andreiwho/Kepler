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

	private:
		TRef<TTextureSampler2D> m_FolderIcon;
		TRef<TTextureSampler2D> m_FileIcon;
		TRef<TTextureSampler2D> m_NavBackIcon;
		TRef<TTextureSampler2D> m_NavFwdIcon;

		AssetTreeNode_Directory* m_CurrentDirectory{nullptr};
		AssetTreeNode_Directory* m_ForwardDirectory{nullptr};
		
		float m_IconSize = 100;
		float m_IconPadding = 16;

		Array<i8> m_SelectionIndexCache{};
	};
}