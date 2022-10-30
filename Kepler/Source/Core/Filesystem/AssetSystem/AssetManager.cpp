#include "AssetManager.h"

#include <filesystem>
#include "../FileUtils.h"

namespace ke
{
	namespace fs = std::filesystem;

	DEFINE_UNIQUE_LOG_CHANNEL(AssetManager, Info);

	AssetManager* AssetManager::Instance;

	AssetManager::AssetManager()
	{
		Instance = this;
		FindGameAssets();
	}

	TFuture<RefPtr<AssetTreeNode>> AssetManager::FindAssetNode(const String& path) const
	{
		return Async(
			[this, Path = path]() -> RefPtr<AssetTreeNode>
			{
				auto pRoot = GetRootNodeFor(Path);
				if (pRoot)
				{
					return pRoot->FindNode(Path);
				}
				return nullptr;
			});
	}

	TFuture<RefPtr<AssetTreeNode>> AssetManager::FindAssetNode(id64 assetId) const
	{
		return Async([this, assetId]() -> RefPtr<AssetTreeNode>
			{
				for (auto& [name, pRoot] : m_Roots)
				{
					if (auto pNode = pRoot->FindNodeById(assetId))
					{
						return pNode;
					}
				}
				return nullptr;
			});
	}

	RefPtr<AssetTreeNode_Directory> AssetManager::GetRootNode(const String& rootPath) const
	{
		if (m_Roots.Contains(rootPath))
		{
			return m_Roots[rootPath];
		}
		return nullptr;
	}

	RefPtr<AssetTreeNode_Directory> AssetManager::GetRootNodeFor(const String& rootPath) const
	{
		if (auto pRoot = GetRootNode(rootPath))
		{
			return pRoot;
		}

		for (const auto& [root, node] : m_Roots)
		{
			if (rootPath.starts_with(root))
			{
				return node;
			}
		}
		return nullptr;
	}

	void AssetManager::FindGameAssets()
	{
		KEPLER_INFO(AssetManager, " ====== Finding asset files... ======");
		const Map<String, String>& vfsAliases = TVirtualFileSystem::Get()->GetPathAliases();
		for (const auto& [key, _] : vfsAliases)
		{
			auto keyToken = key + "://";
			m_Roots[keyToken] = ReadDirectory(keyToken, AssetTreeNode_Directory::New(nullptr, keyToken));
			m_Roots[keyToken]->SetRoot();
		}

		KEPLER_INFO(AssetManager, " ====== Finished finding asset files... ======");
	}

	RefPtr<AssetTreeNode_Directory> AssetManager::ReadDirectory(const String& root, RefPtr<AssetTreeNode_Directory> pDirectory)
	{
		const String rootPath = VFSResolvePath(root);

		for (const auto& entry : fs::directory_iterator(pDirectory->GetPath_Resolved()))
		{
			const fs::path& entryPath = entry.path();
			const fs::path relativePath = fs::relative(entryPath, rootPath);
			String formattedPath = fmt::format("{}{}", root, relativePath.string());
			std::replace(formattedPath.begin(), formattedPath.end(), '\\', '/');

			if (fs::is_directory(entryPath))
			{
				RefPtr<AssetTreeNode_Directory> pNewDirectory = AssetTreeNode_Directory::New(pDirectory.Raw(), formattedPath);
				ReadDirectory(root, pNewDirectory);
				pDirectory->AddChild(pNewDirectory);
			}

			if (fs::is_regular_file(entryPath))
			{
				if (entryPath.has_extension())
				{
					const String extension = entryPath.extension().string();
					if (extension == ".meta")
					{
						auto pNewAsset = AssetTreeNode_AssetMetadata::New(pDirectory.Raw(), formattedPath);
						pDirectory->AddChild(pNewAsset);
					}
					else
					{
						auto pNewAsset = AssetTreeNode_PlainAsset::New(pDirectory.Raw(), formattedPath);
						pDirectory->AddChild(pNewAsset);
					}
				}
			}
		}

		pDirectory->SortChildren(m_DefaultSortingFilter);
		return pDirectory;
	}

}
