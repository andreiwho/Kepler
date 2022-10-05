#include "AssetTree.h"

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

}