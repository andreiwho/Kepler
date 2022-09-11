#include "MeshLoader.h"
#include "Renderer/World/StaticMesh.h"
#include "Core/Filesystem/FileUtils.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "assimp/matrix4x4.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMeshLoader);

	//////////////////////////////////////////////////////////////////////////
	TMeshLoader* TMeshLoader::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	TMeshLoader::TMeshLoader()
	{
		Instance = this;
	}

	//////////////////////////////////////////////////////////////////////////
	TMeshLoader::~TMeshLoader()
	{
		ClearCache();
	}

	//////////////////////////////////////////////////////////////////////////
	void TMeshLoader::ClearCache()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TDynArray<TStaticMeshSection> TMeshLoader::LoadStaticMeshSections(const TString& MeshPath, bool bTryOutputSingleSection)
	{
		Assimp::Importer Importer{};
		TDynArray<u8> Binary = Await(TFileUtils::ReadBinaryFileAsync(MeshPath));
		CHECK(!Binary.IsEmpty());

		const aiScene* pScene = Importer.ReadFileFromMemory(Binary.GetData(), Binary.GetLength(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder |
			aiProcess_GlobalScale |
			(bTryOutputSingleSection ? aiProcess_PreTransformVertices : 0));
		CHECK(pScene);

		TDynArray<TStaticMeshSection> MeshSections;
		MeshSections.Reserve(pScene->mNumMeshes);

		if (ProcessNode(pScene, pScene->mRootNode, MeshSections))
		{
			return MeshSections;
		}
		return {};
	}

	//////////////////////////////////////////////////////////////////////////
	bool TMeshLoader::ProcessNode(const aiScene* pScene, const aiNode* pNode, TDynArray<TStaticMeshSection>& OutSections)
	{
		if (!pNode)
		{
			return false;
		}

		for (u32 Index = 0; Index < pNode->mNumMeshes; ++Index)
		{
			const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[Index]];
			if (!ProcessMesh(pScene, pMesh, pNode->mTransformation, OutSections))
			{
				KEPLER_ERROR(LogMeshLoader, "An error occured while trying to load mesh. Returning empty array...");
				return false;
			}
		}

		for (u32 Index = 0; Index < pNode->mNumChildren; ++Index)
		{
			if (!ProcessNode(pScene, pNode->mChildren[Index], OutSections))
			{
				KEPLER_ERROR(LogMeshLoader, "An error occured while trying to load mesh. Returning empty array...");
				return false;
			}
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TMeshLoader::ProcessMesh(const aiScene* pScene, const aiMesh* pMesh, const aiMatrix4x4& ParentTransform, TDynArray<TStaticMeshSection>& OutSections)
	{
		if (!pMesh || !pScene)
		{
			return false;
		}

		TStaticMeshSection Section{};
		if (!pMesh->HasPositions() || !pMesh->HasTextureCoords(0))
		{
			return false;
		}

		Section.Vertices.Reserve(pMesh->mNumVertices);
		for (u32 Index = 0; Index < pMesh->mNumVertices; ++Index)
		{
			TStaticMeshVertex Vertex{};

			aiVector3D Position = pMesh->mVertices[Index];
			Position *= ParentTransform;
			Vertex.Position = float3(Position.x, Position.y, Position.z);

			const aiVector3D& TexCoord = pMesh->mTextureCoords[0][Index];
			Vertex.UV0 = float2(TexCoord.x, TexCoord.y);

			const aiVector3D& Normal = pMesh->mNormals[Index];
			Vertex.Normal = float3(Normal.x, Normal.y, Normal.z);

			Section.Vertices.EmplaceBack(Vertex);
		}

		Section.Indices.Reserve(std::invoke([pMesh]
			{
				u32 OutVertices = 0;
				for (u32 Index = 0; Index < pMesh->mNumFaces; ++Index)
				{
					const aiFace& pFace = pMesh->mFaces[Index];
					OutVertices += pFace.mNumIndices;
				}
				return OutVertices;
			}
		));

		// Calculate index count
		for (u32 Index = 0; Index < pMesh->mNumFaces; ++Index)
		{
			const aiFace& pFace = pMesh->mFaces[Index];
			for (u32 ElemIndex = 0; ElemIndex < pFace.mNumIndices; ++ElemIndex)
			{
				Section.Indices.EmplaceBack(pFace.mIndices[ElemIndex]);
			}
		}

		OutSections.EmplaceBack(Section);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
}