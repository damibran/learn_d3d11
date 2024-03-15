#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include "Utils/strToWstr.h"

#include "Components/IGameComponent.h"
#include "Components/Transform.h"

#include "Utils/AssimUtils.h"
#include "RenderData/Mesh.h"
#include "RenderData/DiffusionMaterial.h"
#include "RenderData/RasterState.h"

namespace dmbrn {
	/**
	* \brief performs importing of model data from file
	*/
	class ModelComponent :public IGameComponent
	{
	public:
		ModelComponent(GameToComponentBridge bridge, RastState& rs, InputLayout<VertexType>* il, const std::wstring& shaderPath, const std::wstring& path) :
			IGameComponent(bridge),
			rastState(rs),
			shaders(bridge.device.getDevice(), shaderPath),
			constBuf(bridge.device.getDevice(), modelMat)
		{
			Assimp::DefaultLogger::create("", Assimp::DefaultLogger::VERBOSE, aiDefaultLogStream_STDOUT);

			Assimp::Importer importer;
			const aiScene* ai_scene = importer.ReadFile(
				wstrToStr(path).c_str(),
				aiProcess_Triangulate |
				aiProcess_ValidateDataStructure |
				aiProcess_GlobalScale);
			//| aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace aiProcess_FlipUVs |


			if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
			{
				throw std::runtime_error(std::string("ERROR::ASSIMP:: ") + importer.GetErrorString());
			}

			//printAiScene(ai_scene);
			//printAnimations(ai_scene);

			// retrieve the directory path of the filepath
			const std::wstring directory = path.substr(0, path.find_last_of('\\'));
			const std::wstring model_name = path.substr(path.find_last_of('\\') + 1,
				path.find_last_of('.') - path.find_last_of('\\') - 1);

			aiMatrix4x4 root_trans = ai_scene->mRootNode->mTransformation;

			// process ASSIMP's root node recursively
			processNodeData(bridge.device.getDevice(), bridge.device.getContext(), il, ai_scene, ai_scene->mRootNode, directory, model_name, root_trans);

			Assimp::DefaultLogger::kill();
		}

		void Update(float) override
		{
			//
		}
		void RenderDataUpdate() override
		{
			auto mat = constBuf.map(bridge.device.getContext());

			mat->model = transform.getMatrix();

			// no need to transpose
			mat->normModel = transform.getMatrix().Invert();

			mat->model = mat->model.Transpose();
		}
		void Draw() override
		{
			rastState.bind(bridge.device.getContext());
			constBuf.bind(bridge.device.getContext(), 1);
			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			shaders.bindShaders(bridge.device.getContext());

			for (const auto& mesh : meshes)
			{
				mesh.bind(bridge.device.getContext());
				mesh.drawIndexed(bridge.device.getContext());
			}
		}

		TransformComponent transform;

	private:
		RastState& rastState;
		Shaders shaders;

		struct SModelMat {
			DirectX::SimpleMath::Matrix model;
			DirectX::SimpleMath::Matrix normModel;
		} modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;

		std::vector<Mesh> meshes;

		void processNodeData(
			ID3D11Device* device,
			ID3D11DeviceContext* cntx,
			InputLayout<VertexType>* il,
			const aiScene* ai_scene,
			const aiNode* ai_node,
			const std::wstring& directory,
			const std::wstring& parentName,
			const aiMatrix4x4& parentTrans)
		{
			aiMatrix4x4 trans_this = parentTrans * ai_node->mTransformation;

			std::wstring name_this = parentName + L"." + strToWstr(ai_node->mName.C_Str());

			if (ai_node->mNumMeshes)
			{
				// process each mesh located at the current node
				for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
				{
					const aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
					std::wstring mesh_name = name_this + L"." + strToWstr(mesh->mName.C_Str());

					// import as static mesh
					std::wstring ent_mesh_name = strToWstr(mesh->mName.C_Str()) + L":Mesh";

					meshes.emplace_back(Mesh(device, cntx, il, directory, ai_scene, toD3d(parentTrans), mesh));
				}
			}

			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
			{
				processNodeData(device, cntx, il, ai_scene, ai_node->mChildren[i], directory, name_this, trans_this);
			}
		}
	};
}
