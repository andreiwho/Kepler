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

    private:
        void FindGameAssets();
        static TRef<AssetTreeNode_Directory> ReadDirectory(const TString& root, TRef<AssetTreeNode_Directory> pDirectory);
        
        
    private:
        TRef<AssetTreeNode_Directory> m_GameAssetTree{};
        TRef<AssetTreeNode_Directory> m_EngineAssetTree{};
    };
}