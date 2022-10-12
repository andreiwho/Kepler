#include "AssetTree.h"
#include <filesystem>

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogAssetTree, All);

	AssetTreeNode::AssetTreeNode(EAssetNodeType type, AssetTreeNode* pParent, const TString& path)
		: m_Parent(pParent)
		, m_ResolvedPath(VFSResolvePath(path))
		, m_UnresolvedPath(path)
		, m_UUID(path)
		, m_Type(type)
	{
		if (m_ResolvedPath.ends_with('/'))
		{
			m_ResolvedPath = m_ResolvedPath.substr(0, m_ResolvedPath.length() - 1);
		}

		auto fsPath = std::filesystem::path(m_ResolvedPath);
		if (std::filesystem::is_directory(fsPath))
		{
			const auto lastSlash = m_ResolvedPath.find_last_of('/');
			m_Name = m_ResolvedPath.substr(lastSlash + 1);
		}
		else
		{
			if (fsPath.has_stem())
			{
				m_Name = fsPath.stem().string();
			}
		}
	}

	void AssetTreeNode::AddChild(TRef<AssetTreeNode> newChild)
	{
		m_Children.AppendBack(newChild);
		if (newChild->IsDirectory())
		{
			m_bHasDirectories = true;
		}

		KEPLER_INFO(LogAssetTree, "Added child to '{}' '{}' of type '{}'", m_UnresolvedPath, newChild->GetPath(), newChild->GetNodeType().ToString());
	}

	void AssetTreeNode::RemoveChild(TRef<AssetTreeNode> child)
	{
		CRASHMSG("Not implemented");
	}

	void AssetTreeNode::ClearChildren()
	{
		m_Children.Clear();
	}

	void AssetTreeNode::AssignParent(AssetTreeNode* pParent)
	{
		CHECKMSG(!IsRoot(), "Root directory cannot have parent");
		m_Parent = pParent;
	}

	TRef<AssetTreeNode> AssetTreeNode::FindNode(const TString& path)
	{
		return FindNodeById(path);
	}

	TRef<AssetTreeNode> AssetTreeNode::FindNodeById(id64 id)
	{
		if (id == m_UUID)
		{
			return RefFromThis();
		}

		for (const auto& child : m_Children)
		{
			if (child && child->GetUUID() == id)
			{
				return child;
			}

			if (child->IsDirectory())
			{
				if (auto otherChild = child->FindNodeById(id))
				{
					return otherChild;
				}
			}
		}
		return nullptr;
	}

	namespace
	{
		template<EAssetNodeType::EValue CheckedType>
		static bool SortComparator(const TRef<AssetTreeNode>& lhs, const TRef<AssetTreeNode>& rhs)
		{
			if (!lhs || !rhs)
			{
				return false;
			}

			const bool bNamesCorrect = lhs->GetName().length() && rhs->GetName().length();
			const bool bIsOfType = lhs->GetNodeType() == CheckedType;
			if (bNamesCorrect && bIsOfType)
			{
				if (rhs->GetNodeType() != CheckedType)
				{
					return true;
				}

				const TString& lName = lhs->GetName();
				const TString& rName = rhs->GetName();
				const i32 commonLen = (i32)std::min(lName.length(), rName.length());
				CHECK(commonLen > 0);

				for (i32 index = 0; index < commonLen - 1; ++index)
				{
					const char lc = std::tolower(lName[index]);
					const char rc = std::tolower(rName[index]);
					if (lc == rc)
					{
						continue;
					}
					return lc < rc;
				}
			}
			return false;
		}
	}

	void AssetTreeNode::SortChildren(EAssetSortFilter filter)
	{
		switch (filter.Value)
		{
		case EAssetSortFilter::None:
			return;
		case EAssetSortFilter::DirectoriesFirst:
		{
			m_Children.Sort(&SortComparator<EAssetNodeType::Directory>);
		}
		break;
		case EAssetSortFilter::AssetsFirst:
		{
			m_Children.Sort(&SortComparator<EAssetNodeType::PlainAsset>);
		}
		return;
		default:
			break;
		}
	}

}