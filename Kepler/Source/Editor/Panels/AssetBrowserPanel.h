#pragma once
#include "Core/Core.h"
#include "Core/Filesystem/AssetSystem/AssetTree.h"

namespace ke
{
	class TAssetBrowserPanel
	{
	public:
		TAssetBrowserPanel();
		
		void Draw();

	private:
		AssetTreeNode_Directory* m_CurrentDirectory{nullptr};
	};
}