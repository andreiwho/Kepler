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

        TFuture<RefPtr<AssetTreeNode>> FindAssetNode(const TString& path) const;
        static AssetManager* Get() { return Instance; }

        RefPtr<AssetTreeNode_Directory> GetRootNode(const TString& rootPath) const;
        RefPtr<AssetTreeNode_Directory> GetRootNodeFor(const TString& rootPath) const;
        const auto& GetRoots() const& { return m_Roots; }

    private:
        void FindGameAssets();
        static RefPtr<AssetTreeNode_Directory> ReadDirectory(const TString& root, RefPtr<AssetTreeNode_Directory> pDirectory);
        
    private:
        Map<TString, RefPtr<AssetTreeNode_Directory>> m_Roots;

#ifdef ENABLE_EDITOR
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::DirectoriesFirst;
#else
        static constexpr EAssetSortFilter m_DefaultSortingFilter = EAssetSortFilter::AssetsFirst;
#endif
    };
}