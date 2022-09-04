#include "MeshLoader.h"
#include "Renderer/World/StaticMesh.h"
#include <tiny_gltf.h>
#include "Core/Filesystem/FileUtils.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogMeshLoader);

	TMeshLoader* TMeshLoader::Instance = nullptr;

	TMeshLoader::TMeshLoader()
	{
		Instance = this;
	}

	TMeshLoader::~TMeshLoader()
	{
		ClearCache();
	}

	void TMeshLoader::ClearCache()
	{

	}

	TRef<TStaticMesh> TMeshLoader::LoadStaticMesh(const TString& MeshPath)
	{
		tinygltf::TinyGLTF Loader;
		tinygltf::Model LoadedModel;
		TString ErrorString;
		TString Warning;

		bool bResult = false;

		auto Extension = TPath{ MeshPath }.extension();
		if (Extension == ".gltf")
		{
			auto ASCII = Await(TFileUtils::ReadTextFileAsync(MeshPath));
			auto BasePath = VFSGetParentPath(VFSResolvePath(MeshPath));
			bResult = Loader.LoadASCIIFromString(&LoadedModel, &ErrorString, &Warning, ASCII.c_str(), ASCII.length(), BasePath);
			CHECKMSG(bResult, fmt::format("Failed to load model: {}", MeshPath));
		}
		if (Extension == ".glb")
		{
			auto Binary = Await(TFileUtils::ReadBinaryFileAsync(MeshPath));
			auto BasePath = VFSGetParentPath(VFSResolvePath(MeshPath));
			bResult = Loader.LoadBinaryFromMemory(&LoadedModel, &ErrorString, &Warning, Binary.GetData(), Binary.GetLength(), BasePath);
			CHECKMSG(bResult, fmt::format("Failed to load model: {}", MeshPath));
		}

		if (!Warning.empty())
		{
			KEPLER_WARNING(LogMeshLoader, "Warning while loading mesh {}: {}", MeshPath, Warning);
		}

		if (!ErrorString.empty())
		{
			KEPLER_ERROR(LogMeshLoader, "Error while loading mesh {}: {}", MeshPath, ErrorString);
		}

		TDynArray<TStaticMeshVertex> Vertices;
		TDynArray<u32> Indices;
		// TODO: Need to get vertex and index count

		for (auto& glTFMesh : LoadedModel.meshes)
		{
			for (auto& Primitive : glTFMesh.primitives)
			{
				const auto& PositionAccessor = LoadedModel.accessors[Primitive.attributes["POSITION"]];
				const auto& PositionBufferView = LoadedModel.bufferViews[PositionAccessor.bufferView];
				const auto& UVAccessor = LoadedModel.accessors[Primitive.attributes["TEXCOORD_0"]];
				const auto& UVBufferView = LoadedModel.bufferViews[UVAccessor.bufferView];

				const auto& PosBuffer = LoadedModel.buffers[PositionBufferView.buffer];
				const float* Positions = (const float*)&PosBuffer.data[PositionBufferView.byteOffset + PositionAccessor.byteOffset];
				const auto& UVBuffer = LoadedModel.buffers[UVBufferView.buffer];
				const float* UVs = (const float*)&UVBuffer.data[UVBufferView.byteOffset + UVAccessor.byteOffset];

				const auto& IndicesAccessor = LoadedModel.accessors[Primitive.indices];
				const auto& IndexBufferView = LoadedModel.bufferViews[IndicesAccessor.bufferView];
				const auto& IndexBuffer = LoadedModel.buffers[IndexBufferView.buffer];
				const u16* IndexData = (const u16*)&IndexBuffer.data[IndexBufferView.byteOffset + IndicesAccessor.byteOffset];

				for (usize Index = 0; Index < PositionAccessor.count; ++Index)
				{
					TStaticMeshVertex Vertex{};
					Vertex.Position = float3(Positions[Index * 3 + 0], Positions[Index * 3 + 1], Positions[Index * 3 + 2]);
					Vertex.UV0 = float2(UVs[Index * 2 + 0], UVs[Index * 2 + 1]);
					Vertices.EmplaceBack(Vertex);
				}

				for (usize Index = 0; Index < IndicesAccessor.count; ++Index)
				{
					Indices.EmplaceBack(IndexData[Index]);
				}
			}
		}

		return TStaticMesh::New(Vertices, Indices);
	}

}