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

        TRef<AssetTreeNode_Directory> GetRootNode(const TString& rootPath) const;
        TRef<AssetTreeNode_Directory> GetRootNodeFor(const TString& rootPath) const;
        const auto& GetRoots() const& { return m_Roots; }

    private:
        void FindGameAssets();
        static TRef<AssetTreeNode_Directory> ReadDirectory(const TString& root, TRef<AssetTreeNode_Directory> pDirectory);
        
    private:
        Map<TString, TRef<AssetTreeNode_Directory>> m_Roots;

#ifdef ENABLE_EDITOR
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::DirectoriesFirst;
#else
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::AssetsFirst;
#endif
    };
}