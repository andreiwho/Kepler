#pragma once
#include "Core/Core.h"
#include "AssetTree.h"
#include "Async/Async.h"

namespace ke
{
    class AssetManager
    {
        static AssetManager* Instance;
    public:
        AssetManager();

        TFuture<TRef<AssetTreeNode>> FindAssetNode(const TString& path) const;
        static AssetManager* Get() { return Instance; }

        inline TRef<AssetTreeNode_Directory> GetRootNode(const TString& rootPath) const
        {
            if (rootPath.starts_with("Engine://"))
                return m_EngineAssetTree;
            if (rootPath.starts_with("Game://"))
                return m_GameAssetTree;
            CRASH();
        }

    private:
        void FindGameAssets();
        static TRef<AssetTreeNode_Directory> ReadDirectory(const TString& root, TRef<AssetTreeNode_Directory> pDirectory);
        
    private:
        TRef<AssetTreeNode_Directory> m_GameAssetTree{};
        TRef<AssetTreeNode_Directory> m_EngineAssetTree{};

#ifdef ENABLE_EDITOR
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::DirectoriesFirst;
#else
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::AssetsFirst;
#endif
    };
}