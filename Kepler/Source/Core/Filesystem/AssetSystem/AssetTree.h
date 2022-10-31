#pragma once
#include "Core/Core.h"
#include "Reflection/Class.h"

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

		inline String ToString() const
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

		static Array<EAssetNodeType> GetEntries() { return { Root, Directory, AssetMetadata, PlainAsset }; }
	};

	struct EAssetSortFilter
	{
		enum EValue : u8
		{
			None,
			DirectoriesFirst,
			AssetsFirst,
		} Value{};

		constexpr EAssetSortFilter() = default;
		constexpr EAssetSortFilter(EValue value) : Value(value) {}
		inline constexpr operator EValue() const { return Value; }

		inline String ToString() const
		{
			switch (Value)
			{
			case ke::EAssetSortFilter::None:
				return "None";
				break;
			case ke::EAssetSortFilter::DirectoriesFirst:
				return "Directories First";
				break;
			case ke::EAssetSortFilter::AssetsFirst:
				return "Assets First";
				break;
			}
			CRASH();
		}

		static Array<EAssetSortFilter> GetEntries() { return { None, DirectoriesFirst, AssetsFirst }; }
	};

	class AssetTreeNode : public EnableRefPtrFromThis<AssetTreeNode>
	{
	public:
		AssetTreeNode(EAssetNodeType type, AssetTreeNode* pParent, const String& unresolvedPath);
		virtual ~AssetTreeNode() = default;
		inline EAssetNodeType GetNodeType() const { return m_Type; }
		void AddChild(RefPtr<AssetTreeNode> newChild);
		void RemoveChild(RefPtr<AssetTreeNode> child);
		void ClearChildren();
		void AssignParent(AssetTreeNode* pParent);
		RefPtr<AssetTreeNode> FindNode(const String& path);
		RefPtr<AssetTreeNode> FindNodeById(id64 id);

		inline String GetPath() const { return m_UnresolvedPath; }
		inline String GetPath_Resolved() const { return m_ResolvedPath; }

		inline void SetRoot() { CHECK(IsDirectory() || IsRoot()); m_Type = EAssetNodeType::Root; }
		inline bool IsRoot() const { return m_Type == EAssetNodeType::Root; }
		inline bool IsDirectory() const { return m_Type == EAssetNodeType::Directory; }
		inline bool IsAssetMetadata() const { return m_Type == EAssetNodeType::AssetMetadata; }
		inline bool IsPlainAsset() const { return m_Type == EAssetNodeType::PlainAsset; }
		inline id64 GetUUID() const { return m_UUID; }
		inline const Array<RefPtr<AssetTreeNode>>& GetChildren() const
		{
			return m_Children;
		}
		inline const String& GetName() const { return m_Name; }
		inline bool HasDirectories() const { return m_bHasDirectories; }
		inline AssetTreeNode* GetParent() const { return m_Parent; }
		void SortChildren(EAssetSortFilter filter = EAssetSortFilter::None);

	protected:
		template<typename T>
		static RefPtr<T> New(EAssetNodeType type, AssetTreeNode* pParent, const String& unresolvedPath)
		{
			return MakeRef(ke::New<T>(type, pParent, unresolvedPath));
		}

	private:
		AssetTreeNode* m_Parent{};
		Array<RefPtr<AssetTreeNode>> m_Children{};
		// A resolved asset path
		String m_ResolvedPath;
		// An unresolved asset path
		String m_UnresolvedPath;

		id64 m_UUID{};
		EAssetNodeType m_Type{};
		String m_Name{};

		bool m_bHasDirectories = false;
	};

	using Asset = AssetTreeNode;

	class AssetTreeNode_Directory : public AssetTreeNode
	{
	public:
		AssetTreeNode_Directory(EAssetNodeType type, AssetTreeNode* pParent, const String& unresolvedPath)
			:	AssetTreeNode(type, pParent, unresolvedPath)
		{}

		static RefPtr<AssetTreeNode_Directory> New(AssetTreeNode* pParent, const String& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_Directory>(EAssetNodeType::Directory, pParent, unresolvedPath);
		}
	};

	class AssetTreeNode_AssetMetadata : public AssetTreeNode
	{
	public:
		AssetTreeNode_AssetMetadata(EAssetNodeType type, AssetTreeNode* pParent, const String& unresolvedPath)
			: AssetTreeNode(type, pParent, unresolvedPath)
		{}

		static RefPtr<AssetTreeNode_AssetMetadata> New(AssetTreeNode* pParent, const String& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_AssetMetadata>(EAssetNodeType::AssetMetadata, pParent, unresolvedPath);
		}
	};

	/************************************************************************/
	/* Assets                                                               */
	/************************************************************************/
	class AssetTreeNode_PlainAsset: public AssetTreeNode
	{
	public:
		const String& GetExtension() const;

		AssetTreeNode_PlainAsset(EAssetNodeType type, AssetTreeNode* pParent, const String& unresolvedPath);

		static RefPtr<AssetTreeNode_PlainAsset> New(AssetTreeNode* pParent, const String& unresolvedPath)
		{
			return AssetTreeNode::New<AssetTreeNode_PlainAsset>(EAssetNodeType::PlainAsset, pParent, unresolvedPath);
		}

		inline EFieldAssetType GetAssetType() const { return m_AssetType; }

	private:
		String m_Extension{};
		EFieldAssetType m_AssetType{EFieldAssetType::None};
	};
}