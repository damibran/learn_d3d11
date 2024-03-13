#pragma once

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include "Utils/AssimUtils.h"
#include "RenderData/Mesh.h"
#include "RenderData/DiffusionMaterial.h"

namespace dmbrn {
	/**
	* \brief performs importing of model data from file
	*/
	class Model
	{
	public:
		Model(ID3D11Device* device, InputLayout<VertexType>& il, const std::string& path)
		{
			Assimp::DefaultLogger::create("", Assimp::DefaultLogger::VERBOSE, aiDefaultLogStream_STDOUT);

			Assimp::Importer importer;
			const aiScene* ai_scene = importer.ReadFile(
				path,
				aiProcess_Triangulate |
				aiProcess_ValidateDataStructure |
				aiProcess_FlipUVs |
				aiProcess_GlobalScale);
			//| aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace

			if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
			{
				throw std::runtime_error(std::string("ERROR::ASSIMP:: ") + importer.GetErrorString());
			}

			//printAiScene(ai_scene);
			//printAnimations(ai_scene);

			// retrieve the directory path of the filepath
			const std::string directory = path.substr(0, path.find_last_of('\\'));
			const std::string model_name = path.substr(path.find_last_of('\\') + 1,
				path.find_last_of('.') - path.find_last_of('\\') - 1);

			aiMatrix4x4 root_trans = ai_scene->mRootNode->mTransformation;

			// process ASSIMP's root node recursively
			processNodeData(device, il, ai_scene, ai_scene->mRootNode, directory, model_name, root_trans);

			Assimp::DefaultLogger::kill();
		}

	private:
		std::vector<std::pair<DirectX::SimpleMath::Matrix, Mesh>> meshes;

		void processNodeData(
			ID3D11Device* device,
			InputLayout<VertexType>& il,
			const aiScene* ai_scene,
			const aiNode* ai_node,
			const std::string& directory,
			const std::string& parentName,
			const aiMatrix4x4& parentTrans)
		{
			aiMatrix4x4 trans_this = parentTrans * ai_node->mTransformation;

			std::string name_this = parentName + "." + ai_node->mName.C_Str();

			if (ai_node->mNumMeshes)
			{
				// process each mesh located at the current node
				for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
				{
					const aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
					std::string mesh_name = name_this + "." + std::string(mesh->mName.C_Str());

					const aiMaterial* ai_material = ai_scene->mMaterials[mesh->mMaterialIndex];

					const DiffusionMaterial* material = DiffusionMaterial::GetMaterialPtr(device,
						directory, ai_scene, ai_material);

					// import as static mesh
					std::string ent_mesh_name = std::string(mesh->mName.C_Str()) + ":Mesh";

					meshes.push_back({ toD3d(trans_this),Mesh(device,il ,material, mesh_name, mesh) });
				}
			}

			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
			{
				processNodeData(device, il, ai_scene, ai_node->mChildren[i], directory, name_this, trans_this);
			}
		}
	};
}
