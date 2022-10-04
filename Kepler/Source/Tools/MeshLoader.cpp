#include "MeshLoader.h"
#include "Renderer/World/StaticMesh.h"
#include "Core/Filesystem/FileUtils.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "assimp/matrix4x4.h"

namespace ke
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
	Array<TStaticMeshSection> TMeshLoader::LoadStaticMeshSections(const TString& MeshPath, bool bTryOutputSingleSection)
	{
		Assimp::Importer Importer{};
		Array<u8> Binary = Await(TFileUtils::ReadBinaryFileAsync(MeshPath));
		CHECK(!Binary.IsEmpty());

		const aiScene* pScene = Importer.ReadFileFromMemory(Binary.GetData(), Binary.GetLength(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder |
			aiProcess_GlobalScale |
			(bTryOutputSingleSection ? aiProcess_PreTransformVertices : 0));
		CHECK(pScene);

		Array<TStaticMeshSection> MeshSections;
		MeshSections.Reserve(pScene->mNumMeshes);

		if (ProcessNode(pScene, pScene->mRootNode, MeshSections))
		{
			return MeshSections;
		}
		return {};
	}

	//////////////////////////////////////////////////////////////////////////
	bool TMeshLoader::ProcessNode(const aiScene* pScene, const aiNode* pNode, Array<TStaticMeshSection>& OutSections)
	{
		if (!pNode)
		{
			return false;
		}

		for (u32 idx = 0; idx < pNode->mNumMeshes; ++idx)
		{
			const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[idx]];
			if (!ProcessMesh(pScene, pMesh, pNode->mTransformation, OutSections))
			{
				KEPLER_ERROR(LogMeshLoader, "An error occured while trying to load mesh. Returning empty array...");
				return false;
			}
		}

		for (u32 idx = 0; idx < pNode->mNumChildren; ++idx)
		{
			if (!ProcessNode(pScene, pNode->mChildren[idx], OutSections))
			{
				KEPLER_ERROR(LogMeshLoader, "An error occured while trying to load mesh. Returning empty array...");
				return false;
			}
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TMeshLoader::ProcessMesh(const aiScene* pScene, const aiMesh* pMesh, const aiMatrix4x4& ParentTransform, Array<TStaticMeshSection>& OutSections)
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
		for (u32 idx = 0; idx < pMesh->mNumVertices; ++idx)
		{
			TStaticMeshVertex Vertex{};

			aiVector3D Position = pMesh->mVertices[idx];
			Position *= ParentTransform;
			Vertex.Position = float3(Position.x, Position.y, Position.z);

			const aiVector3D& TexCoord = pMesh->mTextureCoords[0][idx];
			Vertex.UV0 = float2(TexCoord.x, TexCoord.y);

			const aiVector3D& Normal = pMesh->mNormals[idx];
			Vertex.Normal = float3(Normal.x, Normal.y, Normal.z);

			Section.Vertices.EmplaceBack(Vertex);
		}

		Section.Indices.Reserve(std::invoke([pMesh]
			{
				u32 OutVertices = 0;
				for (u32 idx = 0; idx < pMesh->mNumFaces; ++idx)
				{
					const aiFace& pFace = pMesh->mFaces[idx];
					OutVertices += pFace.mNumIndices;
				}
				return OutVertices;
			}
		));

		// Calculate index count
		for (u32 idx = 0; idx < pMesh->mNumFaces; ++idx)
		{
			const aiFace& pFace = pMesh->mFaces[idx];
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