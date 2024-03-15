#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Utils/StdUtils.h"

#include <assimp/mesh.h>

#include "RenderData/VertexIndexType.h"
#include "RenderData/InputLayout.h"
#include "RenderData/DeviceLocalBuffer.h"
#include "RenderData/DiffusionMaterial.h"

namespace dmbrn
{
	/**
	 * \brief is just combination of material and static mesh render data
	 */
	class Mesh
	{
	public:
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept = default;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) noexcept = default;

		Mesh(ID3D11Device* device, ID3D11DeviceContext* cntx, InputLayout<VertexType>* il, const std::wstring& directory, const aiScene* scene, const DirectX::SimpleMath::Matrix& parent_trans, const aiMesh* mesh) :
			Mesh(device, cntx, il, directory, scene, mesh, getDataFromMesh(parent_trans, mesh))
		{
		}

		void bind(ID3D11DeviceContext* cntx) const
		{
			material_.bindMaterialData(cntx);
			inputLayout->bind(cntx);
			vertex_buffer_.bindAsVertex(cntx);
			index_buffer_.bindAsIndex(cntx);
		}

		void drawIndexed(ID3D11DeviceContext* cntx)const
		{
			cntx->DrawIndexed(indices_count, 0, 0);
		}

	private:
		DiffusionMaterial material_;
		InputLayout<VertexType>* inputLayout;
		uint32_t indices_count;
		DeviceLocalBuffer<VertexType> vertex_buffer_;
		DeviceLocalBuffer<IndexType> index_buffer_;

		Mesh(ID3D11Device* device, ID3D11DeviceContext* cntx, InputLayout<VertexType>* il, const std::wstring& directory, const aiScene* scene, const aiMesh* mesh, const std::pair<std::vector<VertexType::Object>, std::vector<IndexType::Object>>& vi) :
			material_(device, cntx, directory, scene, scene->mMaterials[mesh->mMaterialIndex]),
			inputLayout(il),
			indices_count(static_cast<uint32_t>(vi.second.size())),
			vertex_buffer_(device, D3D11_BIND_VERTEX_BUFFER, vi.first),
			index_buffer_(device, D3D11_BIND_INDEX_BUFFER, vi.second)
		{
		}

		static std::pair<std::vector<VertexType::Object>, std::vector<IndexType::Object>> getDataFromMesh(const DirectX::SimpleMath::Matrix& parent_trans, const aiMesh* mesh)
		{
			std::vector<VertexType::Object> vertices;
			std::vector<IndexType::Object> indices;
			// walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				VertexType::Object vertex;
				{
					DirectX::SimpleMath::Vector4 vector;
					// we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
					// positions
					vector.x = mesh->mVertices[i].x;
					vector.y = mesh->mVertices[i].y;
					vector.z = mesh->mVertices[i].z;
					vector.w = 1;
					DirectX::SimpleMath::Vector4::Transform(vector, parent_trans, vector);
					vertex.pos = DirectX::SimpleMath::Vector3(vector.x, vector.y, vector.z);
				}
				// normals
				if (mesh->HasNormals())
				{
					DirectX::SimpleMath::Vector3 vector;

					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;

					DirectX::SimpleMath::Vector3::TransformNormal(vector, parent_trans, vector);

					vertex.normal = vector;
				}
				// texture coordinates
				if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					DirectX::SimpleMath::Vector2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.texCoord = vec;
					// tangent
					//vector.x = mesh->mTangents[i].x;
					//vector.y = mesh->mTangents[i].y;
					//vector.z = mesh->mTangents[i].z;
					//vertex.Tangent = vector;
					//// bi tangent
					//vector.x = mesh->mBitangents[i].x;
					//vector.y = mesh->mBitangents[i].y;
					//vector.z = mesh->mBitangents[i].z;
					//vertex.Bitangent = vector;
				}
				else
					vertex.texCoord = DirectX::SimpleMath::Vector2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}
			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return { vertices, indices };
		}

	};
}