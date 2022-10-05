#pragma once
#include "Core/Core.h"

namespace ke
{
	struct EAssetNodeType 
	{
		enum EValue : u8
		{
			// Can be only one instance, has no parent
			Root,

			// May contain meta-assets and assets
			Directory,

			// Can hold only one child, which is asset reference itself
			AssetMetadata,

			// The asset that can be loaded into RAM and manipulated, can't contain children
			PlainAsset,
		} Value{};

		EAssetNodeType() = default;
		EAssetNodeType(EValue value) : Value(value) {}
		inline operator EValue() const { return Value; }

		inline TString ToString() const
		{
			switch (Value)
			{
			case EAssetNodeType::Root:
				return "Root";
				break;
			case EAssetNodeType::Directory:
				return "Directory";
				break;
			case EAssetNodeType::AssetMetadata:
				return "AssetMetadata";
				break;
			case EAssetNodeType::PlainAsset:
				return "Asset";
				break;
			}
			CRASH();
		}
	};

	class AssetTreeNode : public IntrusiveRefCounted
	{
	public:
		AssetTreeNode(EAssetNodeType type, AssetTreeNode* pParent, const TString& unresolvedPath);
		virtual ~AssetTreeNode() = default;
		inline EAssetNodeType GetNodeType() const { return m_Type; }
		void AddChild(TRef<AssetTreeNode> newChild);
		void RemoveChild(TRef<AssetTreeNode> child);
		void AssignParent(AssetTreeNode* pParent);
		TRef<AssetTreeNode> FindChild(const TString& path);
		TRef<AssetTreeNode> FindChildById(id64 id);

		inline TString GetPath() const { return m_UnresolvedPath; }
		inline TString GetPath_Resolved() const { return m_ResolvedPath; }

		inline void SetRoot() { CHECK(IsDirectory() || IsRoot()); m_Type = EAssetNodeType::Root; }
		inline bool IsRoot() const { return m_Type == EAssetNodeType::Root; }
		inline bool IsDirectory() const { return m_Type == EAssetNodeType::Directory; }
		inline bool IsAssetMetadata() const { return m_Type == EAssetNodeType::AssetMetadata; }
		inline bool IsPlainAsset() const { return m_Type == EAssetNodeType::PlainAsset; }
		inline id64 GetUUID() const { return m_UUID; }

		static TRef<AssetTreeNode> MakeRoot(const TString& unresolvedPath)
		{
			return MakeRef(ke::New<AssetTreeNode>(EAssetNodeType::Root, nullptr, unresolvedPath));
		}

	protected:
		template<typename T>
		static TRef<T> New(EAssetNodeType type, AssetTreeNode* pParent, const TString& unresolvedPath)
		{
			return MakeRef(ke::New<T>(type, pParent, unresolvedPath));
		}

	private:
		AssetTreeNode* m_Parent{};
		Array<TRef<AssetTreeNode>> m_Children{};
		// A resolved asset path
		TString m_ResolvedPath;
		// An unresolved asset path
		TString m_UnresolvedPath;

		id64 m_UUID{};
		EAssetNodeType m_Type{};
	};

	class AssetTreeNode_Directory : public AssetTreeNode
	{
	public:
		AssetTreeNode_Directory(EAssetNodeType type, AssetTreeNode* pParent, const TString& unresolvedPath)
			:	AssetTreeNode(type, pParent, unresolvedPath)
		{}

		static TRef<AssetTreeNode_Directory> New(AssetTreeNode* pParent, const TString& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_Directory>(EAssetNodeType::Directory, pParent, unresolvedPath);
		}
	};

	class AssetTreeNode_AssetMetadata : public AssetTreeNode
	{
	public:
		AssetTreeNode_AssetMetadata(EAssetNodeType type, AssetTreeNode* pParent, const TString& unresolvedPath)
			: AssetTreeNode(type, pParent, unresolvedPath)
		{}

		static TRef<AssetTreeNode_AssetMetadata> New(AssetTreeNode* pParent, const TString& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_AssetMetadata>(EAssetNodeType::AssetMetadata, pParent, unresolvedPath);
		}
	};

	class AssetTreeNode_PlainAsset: public AssetTreeNode
	{
	public:
		AssetTreeNode_PlainAsset(EAssetNodeType type, AssetTreeNode* pParent, const TString& unresolvedPath)
			: AssetTreeNode(type, pParent, unresolvedPath)
		{}

		static TRef<AssetTreeNode_PlainAsset> New(AssetTreeNode* pParent, const TString& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_PlainAsset>(EAssetNodeType::PlainAsset, pParent, unresolvedPath);
		}
	};
}