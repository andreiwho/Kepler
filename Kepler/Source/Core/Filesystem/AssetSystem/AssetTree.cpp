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
		, m_Name(std::filesystem::path(path).stem().string())

	{
	}

	void AssetTreeNode::AddChild(TRef<AssetTreeNode> newChild)
	{
		m_Children.AppendBack(newChild);
		KEPLER_INFO(LogAssetTree, "Added child to '{}' '{}' of type '{}'", m_UnresolvedPath, newChild->GetPath(), newChild->GetNodeType().ToString());
	}

	void AssetTreeNode::RemoveChild(TRef<AssetTreeNode> child)
	{
		CRASHMSG("Not implemented");
	}

	void AssetTreeNode::AssignParent(AssetTreeNode* pParent)
	{
		CHECKMSG(!IsRoot(), "Root directory cannot have parent");
		m_Parent = pParent;
	}

	TRef<AssetTreeNode> AssetTreeNode::FindChild(const TString& path)
	{
		return FindChildById(path);
	}

	TRef<AssetTreeNode> AssetTreeNode::FindChildById(id64 id)
	{
		for (const auto& child : m_Children)
		{
			if (child && child->GetUUID() == id)
			{
				return child;
			}

			if (child->IsDirectory())
			{
				if (auto otherChild = child->FindChildById(id))
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