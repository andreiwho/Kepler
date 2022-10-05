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

	TFuture<TRef<AssetTreeNode>> AssetManager::FindAssetNode(const TString& path) const
	{
		if (path.starts_with("Game://"))
		{
			return Async([this, Path = path] { return m_GameAssetTree->FindChild(Path); });;
		}
		else if (path.starts_with("Engine://"))
		{
			return Async([this, Path = path] { return m_EngineAssetTree->FindChild(Path); });;
		}
		CRASH();
	}

	void AssetManager::FindGameAssets()
	{
		KEPLER_INFO(AssetManager, " ====== Finding asset files... ======");
		if (m_GameAssetTree)
		{
			m_GameAssetTree = nullptr;
		}

		m_GameAssetTree = ReadDirectory("Game://", AssetTreeNode_Directory::New(nullptr, "Game://"));
		m_GameAssetTree->SetRoot();

		m_EngineAssetTree = ReadDirectory("Engine://", AssetTreeNode_Directory::New(nullptr, "Engine://"));
		m_EngineAssetTree->SetRoot();

		KEPLER_INFO(AssetManager, " ====== Finished finding asset files... ======");
	}

	TRef<AssetTreeNode_Directory> AssetManager::ReadDirectory(const TString& root, TRef<AssetTreeNode_Directory> pDirectory)
	{
		const TString rootPath = VFSResolvePath(root);

		for (const auto& entry : fs::directory_iterator(pDirectory->GetPath_Resolved()))
		{
			const fs::path& entryPath = entry.path();
			const fs::path relativePath = fs::relative(entryPath, rootPath);
			TString formattedPath = fmt::format("{}{}", root, relativePath.string());
			std::replace(formattedPath.begin(), formattedPath.end(), '\\', '/');

			if (fs::is_directory(entryPath))
			{
				TRef<AssetTreeNode_Directory> pNewDirectory = AssetTreeNode_Directory::New(pDirectory.Raw(), formattedPath);
				ReadDirectory(root, pNewDirectory);
				pDirectory->AddChild(pNewDirectory);
			}

			if (fs::is_regular_file(entryPath))
			{
				if (entryPath.has_extension())
				{
					const TString extension = entryPath.extension().string();
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

		return pDirectory;
	}

}
